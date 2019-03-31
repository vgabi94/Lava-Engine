#include "Material.h"
#include <Manager\PipelineManager.h>
#include <Manager\TextureManager.h>

namespace Engine
{
    MEM_POOL_DEFINE(Material);

    void Material::InitializeUniforms()
    {
        Pipeline& pipeline = PipelineOfType(mPipeType);
        
        uint32_t uniformSize = 0;
        for (const auto& binding : pipeline.mBindings)
        {
            uniformSize = std::max(uniformSize, binding.binding);
        }

        // because binding is 0 indexed
        mUniforms.resize(uniformSize + 1);

        for (const auto& binding : pipeline.mBindings)
        {
            Uniform& uniform = mUniforms[binding.binding];
            uniform.mType = binding.descriptorType;
        }

        mDescSet = pipeline.AllocateDescriptorSet();
        dirty = false;
    }

    void Material::UpdateUniform(uint32_t binding, const std::any& value)
    {
        THROW_IF(binding >= mUniforms.size(), "Uniform binding point out of range {0}!", binding);
        mUniforms[binding].mValue = value;
        dirty = true;
    }

	void Material::UpdateUniform(const std::string& name, const std::any& value)
	{
		Pipeline& pipeline = PipelineOfType(mPipeType);
		THROW_IF(pipeline.mUniforms.find(name) == pipeline.mUniforms.end(), "Uniform name doesn't exist!");
		uint32_t binding = pipeline.mUniforms[name];
		UpdateUniform(binding, value);
	}

    void Material::Bind(vk::CommandBuffer cmdBuff)
    {
        WriteDescriptorsIfDirty();

        const Pipeline& pipeline = PipelineOfType(mPipeType);
        cmdBuff.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
            pipeline.mPipelineLayout, 0, 1, &mDescSet,
            0, nullptr);
    }
    
    void Material::WriteDescriptorsIfDirty()
    {
        if (dirty)
        {
            std::vector<vk::WriteDescriptorSet> writeDescSets;
            writeDescSets.resize(mUniforms.size());
            
            for (size_t i = 0; i < mUniforms.size(); i++)
            {
                if (mUniforms[i].mType == vk::DescriptorType::eCombinedImageSampler ||
                    mUniforms[i].mType == vk::DescriptorType::eSampledImage ||
                    mUniforms[i].mType == vk::DescriptorType::eSampler)
                {
                    Texture tex = TextureAt(std::any_cast<uint32_t>(mUniforms[i].mValue));
                    vk::DescriptorImageInfo imageInfo(tex.mSampler, tex.mImageView,
                        vk::ImageLayout::eShaderReadOnlyOptimal);

                    writeDescSets[i].descriptorCount = 1;
                    writeDescSets[i].descriptorType = mUniforms[i].mType;
                    writeDescSets[i].dstArrayElement = 0;
                    writeDescSets[i].dstBinding = static_cast<uint32_t>(i);
                    writeDescSets[i].dstSet = mDescSet;
                    writeDescSets[i].pImageInfo = &imageInfo;
                }
                else // assume buffer
                {
                    // TODO
                    THROW("Not implemented!");
                }
            }

            g_vkDevice.updateDescriptorSets(writeDescSets, {});
            dirty = false;
        }
    }
}

