using Lava.Engine;
using Lava.Mathematics;
using Lava.Physics;

namespace Demo
{
    static class Program
    {
        static bool fullscreen = false;

        static void Update()
        {
            if (Input.GetKeyPressed(KeyCode.C))
            {
                fullscreen = !fullscreen;
                Application.SetFullScreen(fullscreen);
            }

            if (Input.GetKey(KeyCode.Escape))
            {
                Application.Quit();
            }
        }

        static void InitializeGame()
        {
            //Vertex[] v = new Vertex[3]
            //{
            //    new Vertex{
            //        position = new Vector3(0.0f, -0.5f, 0.0f),
            //        normal = Vector3.UnitZ,
            //        texcoord = new Vector2(1.0f, 0.0f)
            //    },
            //    new Vertex{
            //        position = new Vector3(0.5f, 0.5f, 0.0f),
            //        normal = Vector3.UnitZ,
            //        texcoord = new Vector2(0.0f, 1.0f)
            //    },
            //    new Vertex{
            //        position = new Vector3(-0.5f, 0.5f, 0.0f),
            //        normal = Vector3.UnitZ,
            //        texcoord = new Vector2(0.0f, 0.0f)
            //    }
            //};

            //uint[] i = new uint[3] { 0, 1, 2 };

            //uint tex = Texture.Load2D(Settings.TextureDirPath + "\\texture.jpg");
            uint skyTex = Texture.LoadHDR(Settings.TextureDirPath + "\\sky2.hdr");
            uint skyEnv = Texture.LoadHDR(Settings.TextureDirPath + "\\skyenv2.hdr");

            uint tex = Texture.FromColor(Color.FromHex("#7FFF00"));
            StaticMesh mesh = new StaticMesh(Settings.ModelsDirPath + "\\bunny.obj");
            StaticMesh mesh2 = new StaticMesh(Settings.ModelsDirPath + "\\bunny.obj");
            StaticMesh blocuriMesh = new StaticMesh(Settings.ModelsDirPath + "\\blocuri.obj");
            //StaticMesh mesh = new StaticMesh(v, v.Length, i, i.Length);

            Material material = new Material("phong");
            material.SetUniform(0, tex);
            Material material2 = new Material("phong");
            material2.SetUniform(0, tex);
            Material material3 = new Material("phong");
            material3.SetUniform(0, tex);

            VisualEntity bunny1 = new VisualEntity(mesh, material);
            VisualEntity bunny2 = new VisualEntity(mesh2, material2);
            VisualEntity blocuri = new VisualEntity(blocuriMesh, material3);

            Entity sun = Entity.NewWithComponent(out DirectionalLight sunLight);
            sunLight.Direction = new Vector3(1f, -1f, 0f);

            var trans = bunny2.GetComponent<Transform>();
            trans.Position += Vector3.UnitY * -20f + Vector3.UnitX * -3f;
            trans.Scale *= 2f;
            trans.Rotation *= Quaternion.CreateFromAxisAngle(Vector3.UnitY, Mathf.Radians(90));
            blocuri.Transform.Position += Vector3.UnitX * 5f;

            Entity camEnt = Entity.NewWithComponent(out PerspectiveCamera camera);
            camEnt.AddComponent<CameraController>();
            Camera.Main = camera;
            camera.Position = new Vector3(0f, 0f, 20f);
            camera.FarPlane = 50f;

            World world = WorldManager.CreateWorld();
            world.skySettings.SetColor(Color.GRAY);
            world.skySettings.gamma = 2.2f;
            world.skySettings.exposure = 16f;

            var phys = world.PhysicsWorld;
            //phys.Gravity = new Vector3(0f, -2f, 0f);

            var body = phys.CreateRigidBody();
            body.AddCollisionShape(new SphereShape(2f));
            bunny1.AddComponent(body);

            var body2 = phys.CreateRigidBody(bunny2.Transform.Position, bunny2.Transform.Rotation);
            body2.AddCollisionShape(new BoxShape(2f));
            body2.Type = BodyType.Static;
            bunny2.AddComponent(body2);

            world.AddEntity(bunny1);
            world.AddEntity(bunny2);
            world.AddEntity(camEnt);
            world.AddEntity(blocuri);
            world.AddEntity(sun);

            for (int i = 0; i < 100; i++)
            {
                StaticMesh m = new StaticMesh(Settings.ModelsDirPath + "\\cubplayer.obj");
                Material mat = new Material("phong");
                mat.SetUniform(0, tex);
                VisualEntity visual = new VisualEntity(m, mat);
                visual.Transform.Position = Random.UnitSphere * 100;
                world.AddEntity(visual);
            }

            EventManager.UpdateEvent += Update;
        }

        static void Main(string[] args)
        {
            WindowParams p = new WindowParams(false, "Demo") { Resizable = true };
            Application.Init(p);

            InitializeGame();

            Application.Run();
            System.GC.Collect();
            System.Console.ReadKey();
        }
    }
}
