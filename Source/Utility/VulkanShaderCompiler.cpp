#include "VulkanShaderCompiler.h"
#include "Utility/Logging.h"
#include <glslang/Include/ResourceLimits.h>
#include <glslang/Public/ShaderLang.h>
#include <SPIRV/GlslangToSpv.h>
#include <StandAlone/DirStackFileIncluder.h>
#include <StandAlone/ResourceLimits.h>
#include <spirv.hpp>
#include <spirv_glsl.hpp>
#include <spirv_reflect.hpp>

namespace Blast {

    VulkanShaderCompiler::VulkanShaderCompiler() {
        glslang::InitializeProcess();
    }

    VulkanShaderCompiler::~VulkanShaderCompiler() {
        glslang::FinalizeProcess();
    }

    ShaderCompileResult VulkanShaderCompiler::compile(const ShaderCompileDesc& desc) {
        ShaderCompileResult result;

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
            return result;
        }

        glslang::TProgram program;
        program.addShader(&shader);

        if (!program.link(messages)) {
            BLAST_LOGE("%s\n", program.getInfoLog());
            return result;
        }

        program.mapIO();
        program.buildReflection(EShReflectionAllBlockVariables);

        spv::SpvBuildLogger logger;
        glslang::SpvOptions options;
        options.disableOptimizer = true;
        options.optimizeSize = true;
        glslang::GlslangToSpv(*program.getIntermediate(glslType), result.bytes, &options);

        return result;
    }
}