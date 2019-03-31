using System.Reflection;
using System.Resources;

namespace Lava.Engine
{
    public static class Settings
    {
        private static readonly string LAVA_INI = ".\\LavaEngine.ini";

        public static string ShaderDirPath { get; private set; }
        public static string ShaderSourceDirPath { get; private set; }
        public static string MaterialDirPath { get; private set; }
        public static string PipelineDirPath { get; private set; }
        public static string TextureDirPath { get; private set; }
        public static string ModelsDirPath { get; private set; }

        static Settings()
        {
            //string[] sections = INIHelper.ReadSections(LAVA_INI);
            //if (sections == null)
            //{
            //    ProcessSection(null);
            //}
            //else
            //{
            //    foreach (var section in sections)
            //    {
            //        ProcessSection(section);
            //    }
            //}

            SetGlobals();
            TransferGlobals();
        }

        [System.Obsolete]
        private static void ProcessSection(string section)
        {
            string[] pairs = INIHelper.ReadKeyValuePairs(section, LAVA_INI);
            for (int i = 0; i < pairs.Length; i++)
            {
                ProcessPair(pairs[i]);
            }
        }

        [System.Obsolete]
        private static void ProcessPair(string pair)
        {
            string[] keyvalue = pair.Split('=');
            switch (keyvalue[0])
            {
                case "ShaderDirPath": ShaderDirPath = keyvalue[1]; break;
                case "ShaderSourceDirPath": ShaderSourceDirPath = keyvalue[1]; break;
                case "MaterialDirPath": MaterialDirPath = keyvalue[1]; break;
                case "PipelineDirPath": PipelineDirPath = keyvalue[1]; break;
                default:
                    break;
            }
        }

        private static void SetGlobals()
        {
            ResourceManager rm = Properties.Resources.ResourceManager;
            ShaderDirPath = rm.GetString("ShaderDirectory");
            ShaderSourceDirPath = rm.GetString("ShaderSourceDirectory");
            MaterialDirPath = rm.GetString("MaterialDirectory");
            PipelineDirPath = rm.GetString("PipelineDirectory");
            TextureDirPath = rm.GetString("TextureDirectory");
            ModelsDirPath = rm.GetString("ModelsDirectory");
        }

        private static void TransferGlobals()
        {
            Globals g = new Globals();
            g.MaterialDirPath = MaterialDirPath;
            g.PipelineDirPath = PipelineDirPath;
            g.ShaderDirPath = ShaderDirPath;
            g.ShaderSourcePath = ShaderSourceDirPath;
            g.TexturesPath = TextureDirPath;
            Application.SetGlobals(g);
        }

        public struct Globals
        {
            public string ShaderDirPath;
            public string MaterialDirPath;
            public string PipelineDirPath;
            public string ShaderSourcePath;
            public string TexturesPath;
        }
    }
}
