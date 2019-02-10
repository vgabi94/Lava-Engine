using System;
using Lava.Engine;
using Lava.Mathematics;

namespace Demo
{
    public class CameraController : Component
    {
        private const float CAM_SPEED = 10f;
        private const float MOUSE_SENSITIVITY = 0.5f;

        private bool firstMouse;
        private float lastX, lastY;

        private Camera camera = null;

        protected override void OnInit()
        {
            firstMouse = true;
        }

        protected override void OnUpdate()
        {
            float delta = Time.GetDeltaTime();
            Vector3 translation = Vector3.Zero;

            if (Input.GetKey(KeyCode.A))
                translation += camera.Right * delta * CAM_SPEED;
            if (Input.GetKey(KeyCode.D))
                translation += camera.Right * -delta * CAM_SPEED;
            if (Input.GetKey(KeyCode.W))
                translation += camera.Forward * delta * CAM_SPEED;
            if (Input.GetKey(KeyCode.S))
                translation += camera.Forward * -delta * CAM_SPEED;

            camera.Position += translation;

            Input.GetCursorPosition(out float x, out float y);

            if (Input.GetMouseButtonPressed(MouseButton.Right))
                firstMouse = true;

            MouseMove(x, y);
        }

        protected override void OnWorldAdd()
        {
            camera = Owner.GetComponent<Camera>();
            if (camera == null)
                throw new NullReferenceException("Camera was null");
        }

        private void MouseMove(float x, float y)
        {
            if (Input.GetMouseButton(MouseButton.Right))
            {
                if (firstMouse)
                {
                    lastX = x;
                    lastY = y;
                    firstMouse = false;
                }

                float xoff = lastX - x;
                float yoff = lastY - y;
                lastX = x;
                lastY = y;

                xoff *= MOUSE_SENSITIVITY;
                yoff *= MOUSE_SENSITIVITY;

                LookAround(xoff, -yoff);
            }
        }

        private void LookAround(float yaw, float pitch)
        {
            camera.Rotate(yaw, camera.Up);
            camera.Rotate(pitch, camera.Right);
        }
    }
}
