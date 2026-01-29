using System;
using System.Runtime.InteropServices;
using System.Threading;

public class RadoniumAudio
{
    // Funções nativas do Windows para áudio de baixo nível
    [DllImport("winmm.dll")]
    public static extern int waveOutOpen(out IntPtr hwo, uint uDeviceID, ref WaveFormat lpFormat, IntPtr dwCallback, IntPtr dwInstance, uint dwFlags);
    [DllImport("winmm.dll")]
    public static extern int waveOutPrepareHeader(IntPtr hwo, ref WaveHeader lpWaveOutHdr, uint uSize);
    [DllImport("winmm.dll")]
    public static extern int waveOutWrite(IntPtr hwo, ref WaveHeader lpWaveOutHdr, uint uSize);

    [StructLayout(LayoutKind.Sequential)]
    public struct WaveFormat
    {
        public short wFormatTag; public short nChannels; public int nSamplesPerSec;
        public int nAvgBytesPerSec; public short nBlockAlign; public short wBitsPerSample; public short cbSize;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct WaveHeader
    {
        public IntPtr lpData; public uint dwBufferLength; public uint dwBytesRecorded;
        public IntPtr dwUser; public uint dwFlags; public uint dwLoops; public IntPtr lpNext; public IntPtr reserved;
    }

    private int faseAudio = 1;

    public void Start()
    {
        // Thread para tocar o som
        Thread playbackThread = new Thread(Play);
        playbackThread.IsBackground = true;
        playbackThread.Start();

        // Thread para controlar a mudança de fórmulas a cada 5 segundos
        Thread timerThread = new Thread(() => {
            Thread.Sleep(5000);
            faseAudio = 2;
            Thread.Sleep(5000);
            faseAudio = 3;
        });
        timerThread.IsBackground = true;
        timerThread.Start();
    }

    private void Play()
    {
        int sampleRate = 8000;
        int bufferSize = 4000; // 0.5 segundos por buffer para transição mais rápida

        WaveFormat fmt = new WaveFormat
        {
            wFormatTag = 1,
            nChannels = 1,
            nSamplesPerSec = sampleRate,
            nAvgBytesPerSec = sampleRate,
            nBlockAlign = 1,
            wBitsPerSample = 8,
            cbSize = 0
        };

        if (waveOutOpen(out IntPtr hwo, 0xFFFFFFFF, ref fmt, IntPtr.Zero, IntPtr.Zero, 0) == 0)
        {
            uint t = 0;
            while (true)
            {
                byte[] buffer = new byte[bufferSize];
                for (int i = 0; i < bufferSize; i++)
                {
                    // Seleção da Equação baseada no tempo
                    if (faseAudio == 1)
                        buffer[i] = (byte)((t * (t >> 1 | t >> 56) & 46 & t >> 8) ^ (t & t >> 31 | t >> 6)); // Bytebeat 1
                    else if (faseAudio == 2)
                        buffer[i] = (byte)(t * ((t >> 1 | t >> 2) & (3 & t >> 4))); // Bytebeat 2
                    else
                        buffer[i] = (byte)((t * 5 & t >> 66) | (t * 3 & t >> 12)); // Bytebeat 3
                    t++;
                }

                GCHandle pinnedArray = GCHandle.Alloc(buffer, GCHandleType.Pinned);
                WaveHeader header = new WaveHeader
                {
                    lpData = pinnedArray.AddrOfPinnedObject(),
                    dwBufferLength = (uint)bufferSize
                };

                waveOutPrepareHeader(hwo, ref header, (uint)Marshal.SizeOf(header));
                waveOutWrite(hwo, ref header, (uint)Marshal.SizeOf(header));

                Thread.Sleep(500); // Sincronizado com o tamanho do buffer
                pinnedArray.Free();
            }
        }
    }
}

// Adicionado ponto de entrada explícito para evitar erro de falta de Main.
public static class Program
{
    public static void Main(string[] args)
    {
        var audio = new RadoniumAudio();
        audio.Start();
        Console.WriteLine("Tocando áudio. Pressione Enter para encerrar...");
        Console.ReadLine();
    }
}