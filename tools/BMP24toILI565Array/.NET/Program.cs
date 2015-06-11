using System;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;

namespace BMP24toILI565Array
{
    internal class Program
    {
        static FileStream rgb24file;
        static FileStream rgb16file;
        static StreamWriter rgb16StreamWriter;
        static int bmpWidth, bmpHeight; // W+H in pixels
        static int bmpDepth; // Bit depth (currently must be 24)
        static UInt32 bmpImageoffset; // Start of image data in file
        static int rowSize; // Not always = bmpWidth; may have padding
        static bool flip = true; // BMP is stored bottom-to-top
        static UInt16 row;
        static long pos;


        private static void Main(string[] args)
        {
            //string codeBase = Assembly.GetExecutingAssembly().CodeBase;
            //string exeFilename = Path.GetFileName(codeBase);

            if (args.Length == 0)
            {
                var bmpFiles = Directory.EnumerateFiles(Directory.GetCurrentDirectory(), "*.bmp").ToList();
                if (bmpFiles.Count == 0)
                    Console.WriteLine("\nNo .bmp files found.");
                else
                    bmpFiles.ForEach(i => ConvertImage(i));
            }
            else
            {
                ConvertImage(args[0]);
            }
        }

        static void ConvertImage(string imageFilename)
        {
            if (File.Exists(imageFilename))
            {
                try
                {
                    Console.WriteLine("\nConverting {0}...", imageFilename);
                    rgb24file = new FileStream(imageFilename, FileMode.Open);
                    if (rgb24file == null)
                    {
                        Console.WriteLine("Could not open {0}", imageFilename);
                        return;
                    }

                    rgb24file.Seek(0, SeekOrigin.Begin);

                    if (read16(rgb24file) != 0x4D42)
                    {
                        Console.WriteLine("Not a BMP file");
                        return;
                    }

                    // BMP signature
                    Console.WriteLine("File size: {0}", read32(rgb24file));
                    read32(rgb24file); // Read & ignore creator bytes
                    bmpImageoffset = read32(rgb24file); // Start of image data
                    Console.WriteLine("Image Offset: {0}", bmpImageoffset);
                    // Read DIB header
                    Console.WriteLine("Header size: {0}", read32(rgb24file));
                    bmpWidth = (UInt16)read32(rgb24file);
                    bmpHeight = (UInt16)read32(rgb24file);
                    if (read16(rgb24file) != 1) // # planes -- must be '1'
                    {
                        Console.WriteLine("Number of planes must be 1");
                        return;
                    }

                    bmpDepth = read16(rgb24file); // bits per pixel
                    Console.WriteLine("Bit Depth: {0}", bmpDepth);
                    if (bmpDepth != 24)
                    {
                        Console.WriteLine("Image is not in 24bit");
                        return;
                    }

                    if (read32(rgb24file) != 0) // 0 = uncompressed
                    {
                        Console.WriteLine("BMP must be in uncompressed format");
                        return;
                    }

                    Console.WriteLine("Image size: {0}x{1}", bmpWidth, bmpHeight);

                    string outFilename = Path.ChangeExtension(imageFilename, "h");
                    string imageName = Path.GetFileNameWithoutExtension(imageFilename);

                    Console.WriteLine("{0} created", outFilename);

                    StringBuilder sb = new StringBuilder();

                    sb.AppendLine("#if defined(ARDUINO_ARCH_AVR)");
                    sb.AppendLine("    #include <avr/pgmspace.h>");
                    sb.AppendLine("#elif defined(ARDUINO_SAM_DUE)");
                    sb.AppendLine("    #define PROGMEM");
                    sb.AppendLine("#endif");
                    sb.AppendLine();
                    sb.AppendLine(string.Format("const uint16_t {0}Width = {1};", imageName.ToLower(), bmpWidth));
                    sb.AppendLine(string.Format("const uint16_t {0}Height = {1};", imageName.ToLower(), bmpHeight));
                    sb.AppendLine(string.Format("const uint16_t {0}[{1}] PROGMEM={{", imageName.ToLower(), bmpWidth * bmpHeight));
                    //sb.AppendLine(string.Format("const uint8_t {0}[{1}] PROGMEM={{", imageName.ToLower(), bmpWidth * bmpHeight * 2));

                    rowSize = (bmpWidth * 3 + 3) & ~3;

                    // If bmpHeight is negative, image is in top-down order.
                    // This is not canon but has been observed in the wild.
                    if (bmpHeight < 0)
                    {
                        bmpHeight = -bmpHeight;
                        flip = false;
                    }

                    var inRGB = new byte[3];

                    for (row = 0; row < bmpHeight; row++)
                    {
                        // For each scanline...

                        if (flip) // Bitmap is stored bottom-to-top order (normal BMP)
                            pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
                        else // Bitmap is stored top-to-bottom
                            pos = bmpImageoffset + row * rowSize;

                        rgb24file.Seek(pos, SeekOrigin.Begin);

                        for (var c = 0; c < 3 * bmpWidth; c += 3)
                        {
                            rgb24file.Read(inRGB, 0, 3);

                            int iliColor = to565(inRGB[2], inRGB[1], inRGB[0]);
                            sb.AppendFormat("0x{0},", iliColor.ToString("X4"));
                            //sb.AppendFormat("0x{0},0x{1},", ((byte)(iliColor >> 8)).ToString("X2"), ((byte)(iliColor & 0xFF)).ToString("X2"));
                        }
                        if (row == bmpHeight - 1)
                        {
                            sb.Remove(sb.Length - 1, 1);
                            sb.Append("};");
                        }
                        sb.AppendLine(string.Format(" // row {0}, {1} pixels", row, (row + 1) * bmpWidth));
                    }
                    rgb16file = new FileStream(outFilename, FileMode.Create);
                    rgb16StreamWriter = new StreamWriter(rgb16file);

                    rgb16StreamWriter.Write(sb.ToString());
                }
                catch (Exception ex)
                {
                    Console.WriteLine(ex.Message);
                    Console.WriteLine(ex.StackTrace);
                }
                finally
                {
                    if (rgb24file != null)
                    {
                        rgb24file.Close();
                        rgb24file = null;
                    }
                    if (rgb16StreamWriter != null)
                    {
                        rgb16StreamWriter.Close();
                        rgb16StreamWriter = null;
                    }
                    if (rgb16file != null)
                    {
                        rgb16file.Close();
                        rgb16file = null;
                    }
                }
            }
            else
            {
                Console.WriteLine("File {0} does not exists");
            }
        }

        static private UInt16 to565(byte r, byte g, byte b)
        {
            return (UInt16)(((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3));
            //return ((red >> 3) << 11) | ((green >> 2) << 5) | (blue >> 3);
        }

        static UInt16 read16(FileStream file)
        {
            var bytes = new byte[2];

            file.Read(bytes, 0, 2);
            var word = (UInt16)(bytes[1] << 8 | bytes[0]);

            return word;
        }

        static UInt32 read32(FileStream file)
        {
            var bytes = new byte[4];

            file.Read(bytes, 0, 4);
            var dword = (UInt32)(bytes[3] << 24 | bytes[2] << 16 | bytes[1] << 8 | bytes[0]);

            return dword;
        }
    }


}
