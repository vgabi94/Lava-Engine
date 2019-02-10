using System;
using System.IO;
using System.Diagnostics;

namespace Lava.Engine
{
    public static class ShaderBuilder
    {
        /// <summary>
        /// Build the shaders in the standard shader directory if not build already.
        /// </summary>
        public static void BuildShaders()
        {
            foreach (var file in Directory.EnumerateFiles(Settings.ShaderSourceDirPath))
            {
                string fileName = file.Substring(file.LastIndexOf('\\'));

                if (fileName.Contains(".h")) continue;

                string fileToBuild = Settings.ShaderDirPath + fileName + ".spv";

                if (ShouldBuild(fileToBuild, file))
                {
                    BuildShader(file);
                }
            }
        }

        /// <summary>
        /// Build the shader file.
        /// </summary>
        /// <param name="path">Path to the shader source code file.</param>
        /// <returns>True if the build succeded.</returns>
        private static void BuildShader(string path)
        {
            Console.WriteLine("Building shader: " + path);
            // Make sure the glslc.exe is added to the PATH variable
            string curdir = Directory.GetCurrentDirectory();
            string args = "-c \"" + (curdir + "\\" + path) + "\"";
            ProcessStartInfo startInfo = new ProcessStartInfo("glslc.exe", args);
            startInfo.WorkingDirectory = curdir + "\\" + Settings.ShaderDirPath;
            startInfo.UseShellExecute = false;
            startInfo.CreateNoWindow = true;
            startInfo.RedirectStandardError = true;
            using (var process = new Process())
            {
                process.StartInfo = startInfo;
                process.Start();
                process.WaitForExit();
                string error = process.StandardError.ReadToEnd();
                ConsoleEx.WriteError(error);
            }
        }

        private static bool ShouldBuild(string target, string source)
        {
            if (!File.Exists(target))
                return true;

            DateTime sourceTimestamp = File.GetLastWriteTimeUtc(source);
            DateTime targetTimestamp = File.GetLastWriteTimeUtc(target);

            // If the source file is more recent than the binary file then build
            if (targetTimestamp < sourceTimestamp)
                return true;

            return false;
        }
    }
}
