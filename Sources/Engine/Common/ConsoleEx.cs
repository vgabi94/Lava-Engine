using System;

namespace Lava.Engine
{
    public static class ConsoleEx
    {
        public static void WriteError(string error)
        {
            var color = Console.ForegroundColor;
            Console.ForegroundColor = ConsoleColor.Red;
            Console.Write(error);
            Console.ForegroundColor = color;
        }
    }
}
