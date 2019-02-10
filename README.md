# Lava-Engine
A game engine with Vulkan rendering.

# Prerequisites
* **Visual Studio 2017** or newer.
* **Windows SDK** version **10.0.16299.0** or newer.
* **Vulkan SDK** installed on your computer (**VULKAN_SDK** environment variable **must** be defined!).
* **.NET Framework 4.6.1** or newer.

# Installation
1. Clone or download the repository.
2. Run **GenerateProjects.bat**.
3. The **projects** folder should appear. Run **LavaEngine.sln** from that folder and build the solution (**do not run yet!**).
4. Run **MakeLinks.bat**. Now you can run the project from within Visual Studio!
5. You'll find the binaries in **debug/engine** or **release/engine** depending on which target you chose to build.

*The demo was tested on NVIDIA cards only. I don't know what will happen on AMD cards!*
