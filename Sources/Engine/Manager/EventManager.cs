
namespace Lava.Engine
{
    public static class EventManager
    {
        public delegate void UpdateHandler(); 
        public static event UpdateHandler UpdateEvent;

        public static event UpdateHandler PhysicsUpdateEvent;

        public delegate void FramebufferResizeHandler(int w, int h);
        public static event FramebufferResizeHandler FramebufferResizeEvent;

        internal static void OnUpdate()
        {
            UpdateEvent?.Invoke();
        }

        internal static void OnFramebufferResize(int w, int h)
        {
            FramebufferResizeEvent?.Invoke(w, h);
        }

        internal static void OnPhysicsUpdate()
        {
            PhysicsUpdateEvent?.Invoke();
        }
    }
}
