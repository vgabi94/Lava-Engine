namespace Lava.Engine
{
    /// <summary>
    /// Performs a deep copy of the object and returns the clone
    /// </summary>
    /// <typeparam name="T"></typeparam>
    public interface ICopy<T>
    {
        T Copy();
    }

    /// <summary>
    /// Performs a shallow copy of the object and returns the clone
    /// </summary>
    /// <typeparam name="T"></typeparam>
    public interface IShallowCopy<T>
    {
        T ShallowCopy();
    }
}