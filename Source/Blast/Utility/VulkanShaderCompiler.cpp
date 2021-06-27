#include "VulkanShaderCompiler.h"
#include "Blast/Utility/Logging.h"
#include <glslang/Include/ResourceLimits.h>
#include <glslang/Public/ShaderLang.h>
#include <SPIRV/GlslangToSpv.h>
#include <StandAlone/DirStackFileIncluder.h>
#include <StandAlone/ResourceLimits.h>
#include <spirv.hpp>
#include <spirv_glsl.hpp>
#include <spirv_reflect.hpp>
#include <unordered_set>

namespace Blast {

    struct SpvType {
        uint32_t id;
        uint32_t type_id;
        uint32_t base_type_id;
    };

    enum SpvResourceType {
        SPIRV_TYPE_STAGE_INPUTS = 0,
        SPIRV_TYPE_STAGE_OUTPUTS,
        SPIRV_TYPE_UNIFORM_BUFFERS,
        SPIRV_TYPE_STORAGE_BUFFERS,
        SPIRV_TYPE_IMAGES,
        SPIRV_TYPE_STORAGE_IMAGES,
        SPIRV_TYPE_SAMPLERS,
        SPIRV_TYPE_PUSH_CONSTANT,
        SPIRV_TYPE_SUBPASS_INPUTS,
        SPIRV_TYPE_UNIFORM_TEXEL_BUFFERS,
        SPIRV_TYPE_STORAGE_TEXEL_BUFFERS,
        SPIRV_TYPE_ACCELERATION_STRUCTURES,
        SPIRV_TYPE_COMBINED_SAMPLERS,
        SPIRV_TYPE_COUNT
    };

    enum SpvResourceDim {
        SPIRV_DIM_UNDEFINED = 0,
        SPIRV_DIM_BUFFER = 1,
        SPIRV_DIM_TEXTURE1D = 2,
        SPIRV_DIM_TEXTURE1DARRAY = 3,
        SPIRV_DIM_TEXTURE2D = 4,
        SPIRV_DIM_TEXTURE2DARRAY = 5,
        SPIRV_DIM_TEXTURE2DMS = 6,
        SPIRV_DIM_TEXTURE2DMSARRAY = 7,
        SPIRV_DIM_TEXTURE3D = 8,
        SPIRV_DIM_TEXTURECUBE = 9,
        SPIRV_DIM_TEXTURECUBEARRAY = 10,
        SPIRV_DIM_COUNT = 11,
    };

    struct SpvResource {
        SpvType spv_type;
        SpvResourceType type;
        SpvResourceDim dim;
        bool is_used;
        uint32_t set;
        uint32_t binding;
        uint32_t size;
        std::string name;
    };

    struct SpvVariable {
        uint32_t spv_type_id;
        SpvType parent_spv_type;
        uint32_t parent_index;
        bool is_used;
        uint32_t offset;
        uint32_t size;
        uint32_t count;
        std::string name;
        UniformType type;
    };

    struct CrossCompiler {
        spirv_cross::Compiler* compiler;
        std::vector<SpvResource> resouces;
        std::vector<SpvVariable> variables;
    };

    static ResourceType toGfxResourceType(SpvResourceType type);
    static TextureDimension toGfxResourceDim(SpvResourceDim dim);
    static UniformType toUniformTypr(SpvType type);
    static void createCrossCompiler(const std::vector<uint32_t>& bytes, CrossCompiler* outCompiler);
    static void destroyCrossCompiler(CrossCompiler* inCompiler);
    static void reflectShaderResources(CrossCompiler* inCompiler);
    static void reflectShaderVariables(CrossCompiler* inCompiler);

    VulkanShaderCompiler::VulkanShaderCompiler() {
        glslang::InitializeProcess();
    }

    VulkanShaderCompiler::~VulkanShaderCompiler() {
        glslang::FinalizeProcess();
    }

    ShaderCompileResult VulkanShaderCompiler::compile(const ShaderCompileDesc& desc) {
        ShaderCompileResult result;
        result.success = true;

        EShLanguage glslType;
        switch (desc.stage) {
            case SHADER_STAGE_VERT:
                glslType = EShLangVertex;
                break;
            case SHADER_STAGE_FRAG:
                glslType = EShLangFragment;
                break;
            case SHADER_STAGE_COMP:
                glslType = EShLangCompute;
                break;
            default:
                break;
        }

        glslang::TShader shader(glslType);
        shader.setEnvTarget(glslang::EShTargetSpv, glslang::EShTargetSpv_1_0);

        const char* sourceBytes = desc.code.c_str();
        shader.setStrings(&sourceBytes, 1);
        shader.setEntryPoint("main");
        shader.setPreamble(desc.preamble.c_str());

        DirStackFileIncluder includer;
        for (int i = 0; i < desc.includeDirs.size(); ++i) {
            includer.pushExternalLocalDirectory(desc.includeDirs[i]);
        }

        TBuiltInResource resources = glslang::DefaultTBuiltInResource;
        EShMessages messages = (EShMessages)((int)EShMsgSpvRules | (int)EShMsgVulkanRules);
        if (!shader.parse(&resources, 450, false, messages, includer)) {
            BLAST_LOGE("%s\n", shader.getInfoLog());
            result.success = false;
            return result;
        }

        glslang::TProgram program;
        program.addShader(&shader);

        if (!program.link(messages)) {
            BLAST_LOGE("%s\n", program.getInfoLog());
            result.success = false;
            return result;
        }

        program.mapIO();
        program.buildReflection(EShReflectionAllBlockVariables);

        spv::SpvBuildLogger logger;
        glslang::SpvOptions options;
        options.disableOptimizer = true;
        options.optimizeSize = true;
        glslang::GlslangToSpv(*program.getIntermediate(glslType), result.bytes, &options);

        CrossCompiler cc;
        createCrossCompiler(result.bytes, &cc);
        reflectShaderResources(&cc);
        reflectShaderVariables(&cc);

        for (int i = 0; i < cc.resouces.size(); ++i) {
            GfxShaderResource res;
            res.name = cc.resouces[i].name;
            res.reg = cc.resouces[i].binding;
            res.set = cc.resouces[i].set;
            res.size = cc.resouces[i].size;
            res.dim = toGfxResourceDim(cc.resouces[i].dim);
            res.type = toGfxResourceType(cc.resouces[i].type);
            result.resources.push_back(res);
        }

        for (int i = 0; i < cc.variables.size(); ++i) {
            GfxShaderVariable var;
            var.name = cc.variables[i].name;
            var.parentIndex = cc.variables[i].parent_index;
            var.size = cc.variables[i].size;
            var.offset = cc.variables[i].offset;
            var.count = cc.variables[i].count;
            var.type = cc.variables[i].type;
            result.variables.push_back(var);
        }

        destroyCrossCompiler(&cc);
        return result;
    }

    UniformType toUniformTypr(spirv_cross::SPIRType type) {
        switch (type.basetype) {
            case spirv_cross::SPIRType::Boolean:
                if (type.vecsize == 1 && type.columns == 1) {
                    return UniformType::BOOL;
                } else {
                    return UniformType::UNDEFINED;
                }
                break;
            case spirv_cross::SPIRType::Float:
                if (type.vecsize == 1 && type.columns == 1) {
                    return UniformType::FLOAT;
                } else if (type.vecsize == 2 && type.columns == 1) {
                    return UniformType::FLOAT2;
                } else if (type.vecsize == 3 && type.columns == 1) {
                    return UniformType::FLOAT3;
                } else if (type.vecsize == 4 && type.columns == 1) {
                    return UniformType::FLOAT4;
                } else if (type.vecsize == 3 && type.columns == 3) {
                    return UniformType::MAT3;
                } else if (type.vecsize == 4 && type.columns == 4) {
                    return UniformType::MAT4;
                } else {
                    return UniformType::UNDEFINED;
                }
                break;
            case spirv_cross::SPIRType::Int:
                if (type.vecsize == 1 && type.columns == 1) {
                    return UniformType::INT;
                } else if (type.vecsize == 2 && type.columns == 1) {
                    return UniformType::INT2;
                } else if (type.vecsize == 3 && type.columns == 1) {
                    return UniformType::INT3;
                } else if (type.vecsize == 4 && type.columns == 1) {
                    return UniformType::INT4;
                } else {
                    return UniformType::UNDEFINED;
                }
                break;
            case spirv_cross::SPIRType::UInt:
                if (type.vecsize == 1 && type.columns == 1) {
                    return UniformType::UINT;
                } else if (type.vecsize == 2 && type.columns == 1) {
                    return UniformType::UINT2;
                } else if (type.vecsize == 3 && type.columns == 1) {
                    return UniformType::UINT3;
                } else if (type.vecsize == 4 && type.columns == 1) {
                    return UniformType::UINT4;
                } else {
                    return UniformType::UNDEFINED;
                }
                break;
            default:
                return UniformType::UNDEFINED;
                break;
        }

        return UniformType::UNDEFINED;
    }

    static ResourceType toGfxResourceType(SpvResourceType type) {
        ResourceType result = RESOURCE_TYPE_UNDEFINED;
        switch (type) {
            case SPIRV_TYPE_STAGE_INPUTS:
                result = RESOURCE_TYPE_UNDEFINED;
                break;
            case SPIRV_TYPE_STAGE_OUTPUTS:
                result = RESOURCE_TYPE_UNDEFINED;
                break;
            case SPIRV_TYPE_UNIFORM_BUFFERS:
                result = RESOURCE_TYPE_UNIFORM_BUFFER;
                break;
            case SPIRV_TYPE_STORAGE_BUFFERS:
                result = RESOURCE_TYPE_RW_BUFFER;
                break;
            case SPIRV_TYPE_IMAGES:
                result = RESOURCE_TYPE_TEXTURE;
                break;
            case SPIRV_TYPE_STORAGE_IMAGES:
                result = RESOURCE_TYPE_RW_TEXTURE;
                break;
            case SPIRV_TYPE_SAMPLERS:
                result = RESOURCE_TYPE_SAMPLER;
                break;
            case SPIRV_TYPE_PUSH_CONSTANT:
                // note: 常量不作为Shader资源
                result = RESOURCE_TYPE_UNDEFINED;
                break;
            case SPIRV_TYPE_SUBPASS_INPUTS:
                // note: 暂时不支持subpass input资源
                result = RESOURCE_TYPE_UNDEFINED;
                break;
            case SPIRV_TYPE_UNIFORM_TEXEL_BUFFERS:
                // note: 暂时不支持
                result = RESOURCE_TYPE_UNDEFINED;
                break;
            case SPIRV_TYPE_STORAGE_TEXEL_BUFFERS:
                // note: 暂时不支持
                result = RESOURCE_TYPE_UNDEFINED;
                break;
            case SPIRV_TYPE_ACCELERATION_STRUCTURES:
                // note: 暂时不支持
                result = RESOURCE_TYPE_UNDEFINED;
                break;
            case SPIRV_TYPE_COMBINED_SAMPLERS:
                result = RESOURCE_TYPE_COMBINED_IMAGE_SAMPLER;
                break;
        }
        return result;
    }

    static TextureDimension toGfxResourceDim(SpvResourceDim dim) {
        TextureDimension result = TEXTURE_DIM_UNDEFINED;
        switch (dim) {
            case SPIRV_DIM_UNDEFINED :
                result = TEXTURE_DIM_UNDEFINED;
                break;
            case SPIRV_DIM_BUFFER:
                // note: 暂时不支持
                result = TEXTURE_DIM_UNDEFINED;
                break;
            case SPIRV_DIM_TEXTURE1D:
                result = TEXTURE_DIM_1D;
                break;
            case SPIRV_DIM_TEXTURE1DARRAY:
                result = TEXTURE_DIM_1D_ARRAY;
                break;
            case SPIRV_DIM_TEXTURE2D:
                result = TEXTURE_DIM_2D;
                break;
            case SPIRV_DIM_TEXTURE2DARRAY:
                result = TEXTURE_DIM_2D_ARRAY;
                break;
            case SPIRV_DIM_TEXTURE2DMS:
                result = TEXTURE_DIM_2DMS;
                break;
            case SPIRV_DIM_TEXTURE2DMSARRAY:
                result = TEXTURE_DIM_2DMS_ARRAY;
                break;
            case SPIRV_DIM_TEXTURE3D:
                result = TEXTURE_DIM_3D;
                break;
            case SPIRV_DIM_TEXTURECUBE:
                result = TEXTURE_DIM_CUBE;
                break;
            case SPIRV_DIM_TEXTURECUBEARRAY:
                result = TEXTURE_DIM_CUBE_ARRAY;
                break;
        }
        return result;
    }

    void reflectBoundResources(
            spirv_cross::Compiler* compiler,
            const spirv_cross::SmallVector<spirv_cross::Resource>& allResources,
            const std::unordered_set<uint32_t>& usedResouces,
            std::vector<SpvResource>& resources,
            uint32_t* currentResourceIndex,
            SpvResourceType spvType) {
        for(size_t i = 0; i < allResources.size(); ++i) {
            spirv_cross::Resource const& input = allResources[i];
            SpvResource& resource = resources[(*currentResourceIndex)++];

            resource.spv_type.id = input.id;
            resource.spv_type.type_id = input.type_id;
            resource.spv_type.base_type_id = input.base_type_id;

            resource.type = spvType;

            resource.is_used = (usedResouces.count(resource.spv_type.id) != 0);

            resource.set = compiler->get_decoration(resource.spv_type.id, spv::DecorationDescriptorSet);
            resource.binding = compiler->get_decoration(resource.spv_type.id, spv::DecorationBinding);

            spirv_cross::SPIRType type = compiler->get_type(resource.spv_type.type_id);

            switch (type.image.dim) {
                case spv::DimBuffer:
                    resource.dim = SPIRV_DIM_BUFFER;
                    break;
                case spv::Dim1D:
                    resource.dim = type.image.arrayed ? SPIRV_DIM_TEXTURE1DARRAY : SPIRV_DIM_TEXTURE1D;
                    break;
                case spv::Dim2D:
                    if (type.image.ms)
                        resource.dim = type.image.arrayed ? SPIRV_DIM_TEXTURE2DMSARRAY : SPIRV_DIM_TEXTURE2DMS;
                    else
                        resource.dim = type.image.arrayed ? SPIRV_DIM_TEXTURE2DARRAY : SPIRV_DIM_TEXTURE2D;
                    break;
                case spv::Dim3D:
                    resource.dim = SPIRV_DIM_TEXTURE3D;
                    break;
                case spv::DimCube:
                    resource.dim = type.image.arrayed ? SPIRV_DIM_TEXTURECUBEARRAY : SPIRV_DIM_TEXTURECUBE;
                    break;
                default:
                    resource.dim = SPIRV_DIM_UNDEFINED;
                    break;
            }

            if(type.array.size())
                resource.size = type.array[0];
            else
                resource.size = 1;

            resource.name = compiler->get_name(resource.spv_type.id);
        }
    }

    void createCrossCompiler(const std::vector<uint32_t>& bytes, CrossCompiler* outCompiler) {
        outCompiler->compiler = new spirv_cross::Compiler(bytes.data(), bytes.size());;
        outCompiler->resouces.clear();
        outCompiler->variables.clear();
    }

    void destroyCrossCompiler(CrossCompiler* inCompiler) {
        delete inCompiler->compiler;
        inCompiler->resouces.clear();
        inCompiler->variables.clear();
    }

    void reflectShaderResources(CrossCompiler* inCompiler) {
        spirv_cross::Compiler* compiler = inCompiler->compiler;
        // 1.获取所有Shader资源
        auto allResources = compiler->get_shader_resources();
        std::unordered_set<uint32_t> usedResouces;
        auto tempUsedResouces = compiler->get_active_interface_variables();
        for (auto it = tempUsedResouces.begin(); it != tempUsedResouces.end(); it++) {
            usedResouces.insert(*it);
        }

        // 2.计算Shader资源的数量
        uint32_t resourceCount = 0;
        // todo: inputs和outputs还有push constants
        // resourceCount += (uint32_t)allResources.stage_inputs.size();			    // inputs
        // resourceCount += (uint32_t)allResources.stage_outputs.size();		    // outputs
        // resourceCount += (uint32_t)allResources.push_constant_buffers.size();    // push constants
        resourceCount += (uint32_t)allResources.uniform_buffers.size();		    // const buffers
        resourceCount += (uint32_t)allResources.storage_buffers.size();		    // buffers
        resourceCount += (uint32_t)allResources.separate_images.size();		    // textures
        resourceCount += (uint32_t)allResources.storage_images.size();		    // uav textures
        resourceCount += (uint32_t)allResources.separate_samplers.size();	    // samplers
        resourceCount += (uint32_t)allResources.sampled_images.size();          // combined samplers
        resourceCount += (uint32_t)allResources.subpass_inputs.size();          // input attachments
        resourceCount += (uint32_t)allResources.acceleration_structures.size(); // raytracing structures


        // 3.获取反射信息
        inCompiler->resouces.resize(resourceCount);
        uint32_t currentResourceIndex = 0;

        reflectBoundResources(compiler, allResources.uniform_buffers, usedResouces, inCompiler->resouces, &currentResourceIndex, SPIRV_TYPE_UNIFORM_BUFFERS);
        reflectBoundResources(compiler, allResources.storage_buffers, usedResouces, inCompiler->resouces, &currentResourceIndex, SPIRV_TYPE_STORAGE_BUFFERS);
        reflectBoundResources(compiler, allResources.storage_images, usedResouces, inCompiler->resouces, &currentResourceIndex, SPIRV_TYPE_STORAGE_IMAGES);
        reflectBoundResources(compiler, allResources.separate_images, usedResouces, inCompiler->resouces, &currentResourceIndex, SPIRV_TYPE_IMAGES);
        reflectBoundResources(compiler, allResources.separate_samplers, usedResouces, inCompiler->resouces, &currentResourceIndex, SPIRV_TYPE_SAMPLERS);
        reflectBoundResources(compiler, allResources.sampled_images, usedResouces, inCompiler->resouces, &currentResourceIndex, SPIRV_TYPE_COMBINED_SAMPLERS);
        reflectBoundResources(compiler, allResources.subpass_inputs, usedResouces, inCompiler->resouces, &currentResourceIndex, SPIRV_TYPE_SUBPASS_INPUTS);
        reflectBoundResources(compiler, allResources.subpass_inputs, usedResouces, inCompiler->resouces, &currentResourceIndex, SPIRV_TYPE_SUBPASS_INPUTS);
        reflectBoundResources(compiler, allResources.acceleration_structures, usedResouces, inCompiler->resouces, &currentResourceIndex, SPIRV_TYPE_ACCELERATION_STRUCTURES);
    }

    void reflectShaderVariables(CrossCompiler* inCompiler) {
        if(inCompiler->resouces.size() == 0) {
            return;
        }
        spirv_cross::Compiler* compiler = inCompiler->compiler;

        // 1.获取所有Shader变量数量
        uint32_t variableCount = 0;
        for(uint32_t i = 0; i < inCompiler->resouces.size(); ++i) {
            SpvResource& resource = inCompiler->resouces[i];
            // todo: push constants
            if(resource.type == SPIRV_TYPE_UNIFORM_BUFFERS || resource.type == SPIRV_TYPE_PUSH_CONSTANT) {
                spirv_cross::SPIRType type = compiler->get_type(resource.spv_type.type_id);
                variableCount += (uint32_t)type.member_types.size();
            }
        }

        // 2.获取Shader变量的反射信息
        inCompiler->variables.resize(variableCount);
        uint32_t currentVariableIndex = 0;

        for(uint32_t i = 0; i < inCompiler->resouces.size(); ++i) {
            SpvResource& resource = inCompiler->resouces[i];

            if(resource.type == SPIRV_TYPE_UNIFORM_BUFFERS || resource.type == SPIRV_TYPE_PUSH_CONSTANT) {
                uint32_t startOfBlock = currentVariableIndex;

                spirv_cross::SPIRType type = compiler->get_type(resource.spv_type.type_id);
                for(uint32_t j = 0; j < (uint32_t)type.member_types.size(); ++j) {
                    spirv_cross::SPIRType memberType = compiler->get_type(type.member_types[j]);

                    SpvVariable& variable = inCompiler->variables[currentVariableIndex++];

                    variable.spv_type_id = type.member_types[j];

                    variable.parent_spv_type = resource.spv_type;
                    variable.parent_index = i;

                    variable.is_used = false;

                    variable.size = (uint32_t)compiler->get_declared_struct_member_size(type, j);
                    variable.offset = compiler->get_member_decoration(resource.spv_type.base_type_id, j, spv::DecorationOffset);

                    if (memberType.array.size() > 0) {
                        variable.count = memberType.array[0];
                    } else {
                        variable.count = 1;
                    }

                    variable.type = toUniformTypr(memberType);

                    variable.name = compiler->get_member_name(resource.spv_type.base_type_id, j);
                }

                spirv_cross::SmallVector<spirv_cross::BufferRange> range = compiler->get_active_buffer_ranges(resource.spv_type.id);

                for(uint32_t j = 0; j < (uint32_t)range.size(); ++j) {
                    inCompiler->variables[startOfBlock + range[j].index].is_used = true;
                }
            }
        }
    }
}