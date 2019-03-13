using Sharpmake;

namespace shader2cpp
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
                    Optimization.Debug | Optimization.Release
                )
            };
        }

        public static string BinDebugPath = @"\Tools\Debug";
        public static string BinPath = @"\Tools\Release";

        [Main]
        public static void SharpmakeMain(Arguments args)
        {
            args.Generate<shader2cppSolution>();
        }
    }

    [Generate]
    public class shader2cppProject : Project
    {
        public string BasePath = @"[project.SharpmakeCsPath]\tools\shader2cpp";
        public string Root = @"[project.SharpmakeCsPath]\..";

        public shader2cppProject()
        {
            Name = "shader2cpp";
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

            //conf.IncludePaths.Add(@"[project.BasePath]");
            
            if (target.Optimization == Optimization.Debug)
                conf.TargetPath = @"[project.Root]" + Common.BinDebugPath;
            else
                conf.TargetPath = @"[project.Root]" + Common.BinPath;
            
            conf.IntermediatePath = @"[project.Root]\Temp\[project.Name]\[conf.Name]";
            
            conf.ProjectPath = @"[project.Root]\Projects\[project.Name]";

            conf.Defines.Add("_CRT_SECURE_NO_WARNINGS");
            conf.Defines.Add("LAVA_EXPORTS");

            conf.Options.Add(Options.Vc.General.WindowsTargetPlatformVersion.v10_0_16299_0);
            conf.Options.Add(Options.Vc.Compiler.Exceptions.Enable);
            conf.Options.Add(Options.Vc.Compiler.FloatingPointModel.Precise);
            conf.Options.Add(Options.Vc.Compiler.CppLanguageStandard.Latest);
            conf.Options.Add(Options.Vc.General.WarningLevel.Level3);
        }
    }

    [Generate]
    public class shader2cppSolution : Solution
    {
        public string Root = @"[solution.SharpmakeCsPath]\..";

        public shader2cppSolution()
        {
            Name = "shader2cpp";
            IsFileNameToLower = false;
            AddTargets(Common.GetTargets());
        }

        [Configure()]
        public void Configure(Configuration conf, Target target)
        {
            conf.SolutionPath = @"[solution.Root]\Projects";
            conf.AddProject<shader2cppProject>(target);
        }
    }
}