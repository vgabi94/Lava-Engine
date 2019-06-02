using Lava.Engine;
using Lava.Mathematics;
using Lava.Physics;
using System.Runtime.InteropServices;

namespace Demo
{
    static class Program
    {
        [DllImport("LavaCore.dll")]
        private static extern int GetButtonPressedUI_Native();

        [DllImport("LavaCore.dll")]
        private static extern float GetSliderValueUI_Native();

        [DllImport("LavaCore.dll")]
        private static extern int GetRadioOptionUI_Native();

        static bool fullscreen = false;

        static World mWorld;
        static Material[] mats;
        static VisualEntity[] visuals;
        static PhysicsWorld phys;
        static Entity sun;

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

            if (Input.GetKeyPressed(KeyCode.E))
            {
                StaticMesh m = new StaticMesh(Settings.ModelsDirPath + "\\Crate1.obj");
                int i = Random.Range(0, 4);
                VisualEntity visual = new VisualEntity(m, mats[i]);
                visual.Transform.Position = Camera.Main.Position + Camera.Main.Forward * 5f;

                var bdy = phys.CreateRigidBody(visual.Transform.Position);
                bdy.AddCollisionShape(new BoxShape(1.1f));
                visual.AddComponent(bdy);

                //var cb = phys.CreateCollisionBody(visual.Transform.Position);
                //var trigShape = new BoxShape(1.5f, true);
                //trigShape.CollisionEvent += TrigShape_CollisionEvent;
                //cb.AddCollisionShape(trigShape);
                //visual.AddComponent(cb);

                mWorld.AddEntity(visual);

                bdy.ApplyForceToCenterOfMass(Camera.Main.Forward * 5000f);
            }

            float intensity = GetSliderValueUI_Native();
            mWorld.skySettings.exposure  = intensity * 32f;

            if (GetButtonPressedUI_Native() == 1)
            {
                for (int i = 0; i < visuals.Length; i++)
                {
                    float power = 300f;
                    if (GetRadioOptionUI_Native() == 1)
                        power = 800f;
                    visuals[i].GetComponent<RigidBody>().ApplyForceToCenterOfMass(Vector3.UnitY * power);
                }
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

            mats = new Material[]
            {
                Material.FromJSON(Settings.MaterialDirPath + "\\rustediron.mat.json"),
                Material.FromJSON(Settings.MaterialDirPath + "\\iron.mat.json"),
                Material.FromJSON(Settings.MaterialDirPath + "\\granite.mat.json"),
                Material.FromJSON(Settings.MaterialDirPath + "\\grass.mat.json"),
                Material.FromJSON(Settings.MaterialDirPath + "\\redplastic.mat.json")
            };

            //uint tex = Texture.Load2D(Settings.TextureDirPath + "\\texture.jpg");
            uint skyTex = Texture.LoadHDR(Settings.TextureDirPath + "\\sky2.hdr", true);
            uint skyEnv = Texture.LoadHDR(Settings.TextureDirPath + "\\skyenv2.hdr");

            uint tex = Texture.FromColor(Color.FromHex("#7FFF00"));
            //uint floorColor = Texture.FromColor(Color.FromHex("#dddddd"));
            //StaticMesh mesh = new StaticMesh(Settings.ModelsDirPath + "\\bunny.obj");
            //StaticMesh mesh2 = new StaticMesh(Settings.ModelsDirPath + "\\bunny.obj");
            StaticMesh crate1 = new StaticMesh(Settings.ModelsDirPath + "\\Crate1.obj");
            StaticMesh crate2 = new StaticMesh(Settings.ModelsDirPath + "\\Crate1.obj");
            //StaticMesh mesh = new StaticMesh(v, v.Length, i, i.Length);

            Material material = new Material("phong");
            material.SetUniform(0, tex);
            Material material2 = new Material("phong");
            material2.SetUniform(0, tex);
            Material material3 = new Material("phong");
            material3.SetUniform(0, tex);
            Material material4 = Material.FromJSON(Settings.MaterialDirPath + "\\rustediron.mat.json");
            Material material5 = Material.FromJSON(Settings.MaterialDirPath + "\\iron.mat.json");

            //VisualEntity bunny1 = new VisualEntity(mesh, material);
            //VisualEntity bunny2 = new VisualEntity(mesh2, material2);
            VisualEntity crate1Ent = new VisualEntity(crate1, material4);
            VisualEntity crate2Ent = new VisualEntity(crate2, material5);

            sun = Entity.NewWithComponent(out DirectionalLight sunLight);
            sunLight.Direction = new Vector3(1f, 0.8f, 0.5f);

            var catTrans = crate1Ent.GetComponent<Transform>();
            catTrans.Position += Vector3.UnitY * -10f + Vector3.UnitX * -5f;
            catTrans.Scale *= 1f;

            //var trans = bunny2.GetComponent<Transform>();
            //trans.Position += Vector3.UnitY * -20f + Vector3.UnitX * -3f;
            //trans.Scale *= 2f;
            //trans.Rotation *= Quaternion.CreateFromAxisAngle(Vector3.UnitY, Mathf.Radians(90));

            Entity camEnt = Entity.NewWithComponent(out PerspectiveCamera camera);
            camEnt.AddComponent<CameraController>();
            Camera.Main = camera;
            camera.Position = new Vector3(0f, 0f, 20f);
            camera.FarPlane = 512f;

            mWorld = WorldManager.CreateWorld();
            mWorld.skySettings.SetColor(Color.GRAY);
            mWorld.skySettings.gamma = 2.2f;
            mWorld.skySettings.exposure = 16f;
            mWorld.skySettings.ambient = 0.2f;
            mWorld.skySettings.hdrTex = skyTex;
            mWorld.skySettings.hdrEnv = skyEnv;

            phys = mWorld.PhysicsWorld;
            //phys.Gravity = new Vector3(0f, -2f, 0f);

            var crateBody = phys.CreateRigidBody(crate1Ent.Transform.Position);
            crateBody.AddCollisionShape(new BoxShape(1.1f));
            crate1Ent.AddComponent(crateBody);

            var crate2Body = phys.CreateRigidBody(crate2Ent.Transform.Position);
            crate2Body.AddCollisionShape(new BoxShape(1.1f));
            crate2Ent.AddComponent(crate2Body);

            //var body = phys.CreateRigidBody();
            //body.AddCollisionShape(new SphereShape(2f));
            //bunny1.AddComponent(body);

            //var body2 = phys.CreateRigidBody(bunny2.Transform.Position, bunny2.Transform.Rotation);
            //body2.AddCollisionShape(new BoxShape(2f));
            //body2.Type = BodyType.Static;
            //bunny2.AddComponent(body2);

            //mWorld.AddEntity(bunny1);
            //mWorld.AddEntity(bunny2);
            mWorld.AddEntity(camEnt);
            //mWorld.AddEntity(blocuri);
            mWorld.AddEntity(sun);
            mWorld.AddEntity(crate1Ent);
            mWorld.AddEntity(crate2Ent);

            // for now just keep the music in Models directory
            AudioClip clip = new AudioClip(Settings.ModelsDirPath + "\\ChillingMusic.wav");
            Entity music = new Entity();
            music.AddComponent(clip);
            mWorld.AddEntity(music);
            clip.Play();
            clip.Looped = true;

            visuals = new VisualEntity[10];
            for (int i = 0; i < 10; i++)
            {
                StaticMesh m = new StaticMesh(Settings.ModelsDirPath + "\\Crate1.obj");
                //Material mat;
                //if (Random.Range01() > 0.5)
                //    mat = Material.FromJSON(Settings.MaterialDirPath + "\\rustediron.mat.json");
                //else
                //    mat = Material.FromJSON(Settings.MaterialDirPath + "\\iron.mat.json");

                visuals[i] = new VisualEntity(m, mats[i % mats.Length]);
                VisualEntity visual = visuals[i];
                visual.Transform.Position = Random.UnitSphere * 10;
                var bdy = phys.CreateRigidBody(visual.Transform.Position);
                bdy.AddCollisionShape(new BoxShape(1.1f));
                visual.AddComponent(bdy);

                var cb = phys.CreateCollisionBody(visual.Transform.Position);
                var trigShape = new BoxShape(1.5f, true);
                trigShape.CollisionEvent += TrigShape_CollisionEvent;
                cb.AddCollisionShape(trigShape);
                visual.AddComponent(cb);

                mWorld.AddEntity(visual);
            }

            StaticMesh floor = new StaticMesh(Settings.ModelsDirPath + "\\Crate1.obj");
            Material floorMat = Material.FromJSON(Settings.MaterialDirPath + "\\mahogfloor.mat.json");
            VisualEntity floorVisual = new VisualEntity(floor, floorMat);
            floorVisual.Transform.Position = new Vector3(0f, -30f, 0f);
            floorVisual.Transform.Scale = new Vector3(100f, 0.2f, 100f);

            var floorBody = phys.CreateRigidBody(floorVisual.Transform.Position, floorVisual.Transform.Rotation);
            floorBody.AddCollisionShape(new BoxShape(50f, 0.1f, 50f));
            floorBody.Type = BodyType.Static;
            floorVisual.AddComponent(floorBody);
            mWorld.AddEntity(floorVisual);

            Entity iblEnt = Entity.NewWithComponent(out IBLProbe iblProbe);
            iblProbe.Position = Vector3.Zero;
            mWorld.AddEntity(iblEnt);

            EventManager.UpdateEvent += Update;
        }

        private static void TrigShape_CollisionEvent(CollisionInfo collisionInfo)
        {
            
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
