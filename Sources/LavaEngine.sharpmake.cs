using Sharpmake;

namespace LavaEngine
{
    static class Common
    {
        public static ITarget[] GetTargets()
        {
            return new[]
            {
                new Target(
                    Platform.win64,
                    DevEnv.vs2017,
                    Optimization.Debug | Optimization.Release,
                    framework: DotNetFramework.v4_6_1
                )
            };
        }

        public static string BinDebugPath = @"\Debug\Engine";
        public static string BinPath = @"\Release\Engine";

        [Main]
        public static void SharpmakeMain(Arguments args)
        {
            args.Generate<LavaEngineSolution>();
        }
    }

    [Generate]
    public class LavaCoreProject : Project
    {
        public string BasePath = @"[project.SharpmakeCsPath]\Core";
        public string Root = @"[project.SharpmakeCsPath]\..";

        public LavaCoreProject()
        {
            Name = "LavaCore";
            SourceRootPath = "[project.BasePath]";
            RootPath = "[project.Root]";
            IsFileNameToLower = false;
            IsTargetFileNameToLower = false;
            AddTargets(Common.GetTargets());
        }

        [Configure()]
        public void Configure(Configuration conf, Target target)
        {
            conf.Output = Configuration.OutputType.Dll;
            
            conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\extern");
            conf.IncludePaths.Add(@"$(VULKAN_SDK)\Include");
            conf.IncludePaths.Add(@"[project.BasePath]");

            conf.LibraryPaths.Add(@"[project.Root]\Dependencies");
            conf.LibraryPaths.Add(@"$(VULKAN_SDK)\Lib");
            
            if (target.Optimization == Optimization.Debug)
                conf.TargetPath = @"[project.Root]" + Common.BinDebugPath;
            else
                conf.TargetPath = @"[project.Root]" + Common.BinPath;
            
            conf.IntermediatePath = @"[project.Root]\Temp\[project.Name]\[conf.Name]";
            
            conf.ProjectPath = @"[project.Root]\Projects\[project.Name]";

            if (target.Optimization == Optimization.Debug)
                conf.LibraryFiles.Add("reactphysics3d_d");
            else
                conf.LibraryFiles.Add("reactphysics3d");

            conf.LibraryFiles.Add("vulkan-1");
            conf.LibraryFiles.Add("glfw3");
            conf.LibraryFiles.Add("assimp");

            conf.Defines.Add("_CRT_SECURE_NO_WARNINGS");
            conf.Defines.Add("LAVA_EXPORTS");

            conf.Options.Add(Options.Vc.General.WindowsTargetPlatformVersion.v10_0_16299_0);
            conf.Options.Add(Options.Vc.Compiler.Exceptions.Enable);
            conf.Options.Add(Options.Vc.Compiler.FloatingPointModel.Precise);
            conf.Options.Add(Options.Vc.Compiler.CppLanguageStandard.Latest);
            conf.Options.Add(Options.Vc.General.WarningLevel.Level3);
            
            if (target.Optimization == Optimization.Debug)
            {
                conf.Options.Add(Options.Vc.Compiler.RuntimeChecks.Both);
                conf.Options.Add(Options.Vc.Compiler.RuntimeLibrary.MultiThreadedDebugDLL);
            }
            else
            {
                conf.Options.Add(Options.Vc.Compiler.RuntimeLibrary.MultiThreadedDLL);
                conf.Options.Add(Options.Vc.General.WholeProgramOptimization.LinkTime);
            }
        }
    }

    [Generate]
    public class LavaEngineProject : CSharpProject
    {
        public string BasePath = @"[project.SharpmakeCsPath]\Engine";
        public string Root = @"[project.SharpmakeCsPath]\..";

        public LavaEngineProject()
        {
            Name = "LavaEngine";
            RootPath = "[project.Root]";
            SourceRootPath = "[project.BasePath]";
            RootNamespace = "Lava.Engine";
            IsFileNameToLower = false;
            IsTargetFileNameToLower = false;
            DependenciesCopyLocal = DependenciesCopyLocalTypes.None;
            AddTargets(Common.GetTargets());
        }

        [Configure()]
        public void Configure(Configuration conf, Target target)
        {
            conf.Output = Configuration.OutputType.DotNetClassLibrary;
            conf.ProjectPath = @"[project.Root]\Projects\[project.Name]";
            conf.IntermediatePath = @"[project.Root]\Temp\[project.Name]\[conf.Name]";
            conf.Defines.Add("COLUMN_MAJOR");
            
            if (target.Optimization == Optimization.Debug)
                conf.TargetPath = @"[project.Root]" + Common.BinDebugPath;
            else
                conf.TargetPath = @"[project.Root]" + Common.BinPath;

            conf.ReferencesByName.AddRange(new Strings("System",
                                                        "System.Core",
                                                        "System.Xml.Linq",
                                                        "System.Data.DataSetExtensions",
                                                        "System.Data",
                                                        "System.Xml",
                                                        "Microsoft.CSharp",
                                                        "System.Net.Http"));
            conf.AddPrivateDependency<LavaCoreProject>(target, DependencySetting.OnlyBuildOrder);
        }
    }

    [Generate]
    public class DemoProject : CSharpProject
    {
        public string BasePath = @"[project.SharpmakeCsPath]\Demo";
        public string Root = @"[project.SharpmakeCsPath]\..";

        public DemoProject()
        {
            Name = "Demo";
            RootPath = "[project.Root]";
            SourceRootPath = "[project.BasePath]";
            IsFileNameToLower = false;
            IsTargetFileNameToLower = false;
            DependenciesCopyLocal = DependenciesCopyLocalTypes.None;
            AddTargets(Common.GetTargets());
        }

        [Configure()]
        public void Configure(Configuration conf, Target target)
        {
            conf.Output = Configuration.OutputType.DotNetConsoleApp;
            conf.ProjectPath = @"[project.Root]\Projects\[project.Name]";
            conf.IntermediatePath = @"[project.Root]\Temp\[project.Name]\[conf.Name]";

            if (target.Optimization == Optimization.Debug)
            {
                conf.TargetPath = @"[project.Root]" + Common.BinDebugPath;
                conf.CsprojUserFile = new Configuration.CsprojUserFileSettings();
                conf.CsprojUserFile.EnableUnmanagedDebug = true;
            }
            else
            {
                conf.TargetPath = @"[project.Root]" + Common.BinPath;
            }

            conf.ReferencesByName.AddRange(new Strings("System",
                                                        "System.Core",
                                                        "System.Xml.Linq",
                                                        "System.Data.DataSetExtensions",
                                                        "System.Data",
                                                        "System.Xml",
                                                        "Microsoft.CSharp",
                                                        "System.Net.Http"));
            
            conf.ReferencesByPath.Add(conf.TargetPath + @"\LavaEngine.dll");
            conf.AddPrivateDependency<LavaEngineProject>(target, DependencySetting.OnlyBuildOrder);
        }
    }

    [Generate]
    public class LavaEngineSolution : Solution
    {
        public string Root = @"[solution.SharpmakeCsPath]\..";

        public LavaEngineSolution()
        {
            Name = "LavaEngine";
            IsFileNameToLower = false;
            AddTargets(Common.GetTargets());
        }

        [Configure()]
        public void Configure(Configuration conf, Target target)
        {
            conf.SolutionPath = @"[solution.Root]\Projects";
            conf.AddProject<DemoProject>(target);
        }
    }
}