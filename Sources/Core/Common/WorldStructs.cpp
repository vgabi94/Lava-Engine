#include "WorldStructs.h"
#include <Manager\ResourceManager.h>

namespace Engine
{
	uint32_t IBLProbe::GetIrradMap() const
	{
		return (uint32_t)-1;
	}
	
	uint32_t IBLProbe::GetPrefEnvMap() const
	{
		return g_ResourceManager.GetPrefEnvMap(mResIndex);
	}
	
	uint32_t IBLProbe::GetBrdfMap() const
	{
		return g_ResourceManager.GetBrdfMap(mResIndex);
	}
}
