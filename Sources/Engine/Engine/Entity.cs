using System.Collections.Generic;

namespace Lava.Engine
{
    public class Entity
    {
        public Entity()
        {
            components = new List<Component>();
        }

        public World World
        {
            get { return world; }
            internal set
            {
                foreach (var comp in components)
                {
                    if (world != null)
                        comp.OnWorldRemove();
                    world = value;
                    comp.OnWorldAdd();
                }
            }
        }
        private World world;

        public void AddComponent(Component comp)
        {
            components.Add(comp);
            comp.Owner = this;
            if (world != null)
                comp.OnWorldAdd();
        }

        public T AddComponent<T>() where T : Component, new()
        {
            T comp = new T { Owner = this };
            if (world != null)
                comp.OnWorldAdd();
            components.Add(comp);
            return comp;
        }

        public void RemoveComponent(Component comp)
        {
            if (world != null)
                comp.OnWorldRemove();
            comp.Owner = null;
            components.Remove(comp);
        }

        public bool HasComponent(Component comp)
        {
            return components.Contains(comp);
        }

        public bool HasComponent<T>() where T : Component
        {
            return components.Exists(x => x is T);
        }

        public T GetComponent<T>() where T : Component
        {
            return components.Find(x => x is T) as T;
        }

        private List<Component> components;

        internal virtual void Update()
        {
            foreach (var comp in components)
            {
                comp.OnUpdate();
            }
        }

        /// <summary>
        /// Creates a new entity with the specified component.
        /// </summary>
        /// <typeparam name="T">The type of the component to add to the entity</typeparam>
        /// <param name="comp">The component added to the entity.</param>
        public static Entity NewWithComponent<T>(out T comp) where T : Component, new()
        {
            Entity entity = new Entity();
            comp = entity.AddComponent<T>();
            return entity;
        }

        /// <summary>
        /// Creates a new entity with the specified component.
        /// </summary>
        /// <typeparam name="T">The type of the component to add to the entity</typeparam>
        public static Entity NewWithComponent<T>() where T : Component, new()
        {
            Entity entity = new Entity();
            entity.AddComponent<T>();
            return entity;
        }
    }
}
