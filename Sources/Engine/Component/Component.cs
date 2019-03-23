using System;
using System.Collections.Generic;

namespace Lava.Engine
{
    public abstract class Component
    {
        // Called when the object is constructing. Means is not fully constructed!
        protected internal virtual void OnInit() { }

        // When added to the world
        protected internal virtual void OnWorldAdd() { }

        protected internal virtual void OnEntityAddOwner() { }

        protected internal virtual void OnUpdate() { }

        protected internal virtual void OnPhysicsUpdate() { }

        // When removed from the world
        protected internal virtual void OnWorldRemove() { }

        protected internal virtual void OnEntityRemoveOwner() { }

        protected internal virtual void OnDestroy() { }

        public Entity Owner
        {
            get { return owner; }
            internal set
            {
                if (value == null)
                {
                    OnEntityRemoveOwner();
                    owner = value;
                    //MarkAsOrphan();
                }
                else
                {
                    owner = value;
                    OnEntityAddOwner();
                    //RemoveOrphan();
                }
                
            }
        }
        private Entity owner;

        public bool IsOrphan
        {
            get { return orphans.Contains(this); }
        }

        // WARNING: This constructor does not mark orphans!
        protected Component()
        {
            owner = null;
            EventManager.PhysicsUpdateEvent += OnPhysicsUpdate;
            OnInit();
        }

        public void Destroy()
        {
            owner?.RemoveComponent(this);
            RemoveOrphan();
        }

        public void DestroyForced()
        {
            Destroy();
            GC.Collect(0);
        }

        private void MarkAsOrphan()
        {
            if (!orphans.Contains(this))
                orphans.Add(this);
        }

        private void RemoveOrphan()
        {
            orphans.Remove(this);
        }

        private static HashSet<Component> orphans = new HashSet<Component>();

        ~Component()
        {
            EventManager.PhysicsUpdateEvent -= OnPhysicsUpdate;
            OnDestroy();
        }
    }
}
