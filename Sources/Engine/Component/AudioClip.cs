using System;
using System.Runtime.InteropServices;
using Lava.Mathematics;

namespace Lava.Engine
{
    public class AudioClip : Component
    {
        [DllImport("LavaCore.dll")]
        private static extern IntPtr AddSound_Native(string path);

        [DllImport("LavaCore.dll")]
        private static extern void PlaySound_Native(IntPtr sound);

        [DllImport("LavaCore.dll")]
        private static extern void StopSound_Native(IntPtr sound);

        [DllImport("LavaCore.dll")]
        private static extern void LoopSound_Native(IntPtr sound, bool loop);

        [DllImport("LavaCore.dll")]
        private static extern void SoundSetVolume_Native(IntPtr sound, float volume);

        public IntPtr NativePtr { get; internal set; }

        public AudioClip(string path)
        {
            NativePtr = AddSound_Native(path);
            IsPlaying = false;
        }

        public void Play()
        {
            PlaySound_Native(NativePtr);
        }

        public void Stop()
        {
            StopSound_Native(NativePtr);
        }

        public bool IsPlaying { get; internal set; }

        /// <summary>
        /// Set if the sound is looping or not
        /// </summary>
        public bool Looped
        {
            get => looped;
            set
            {
                looped = value;
                LoopSound_Native(NativePtr, value);
            }
        }
        private bool looped;

        /// <summary>
        /// Set volume (range [0, 1])
        /// </summary>
        public float Volume
        {
            get => volume;
            set
            {
                volume = Mathf.Clamp01(value);
                SoundSetVolume_Native(NativePtr, volume);
            }
        }
        private float volume;
    }
}
