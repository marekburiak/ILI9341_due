using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.IO.Ports;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace ILIScreenshotViewer
{
    public partial class Form1 : Form
    {
        private bool loadImageFromData = false;
        private bool _isLandscape;
        private bool isLandscape
        {
            get
            {
                return _isLandscape;
            }
            set
            {
                if (_isLandscape != value)
                {
                    _isLandscape = value;
                    pictureBoxL.Visible = value;
                    pictureBoxP.Visible = !value;
                }
            }
        }

        private PictureBox pictureBox
        {
            get
            {
                if (isLandscape)
                    return pictureBoxL;
                else
                    return pictureBoxP;
            }
        }

        public Form1()
        {
            InitializeComponent();
        }

        private void btnShow_Click(object sender, EventArgs e)
        {


            //var hexString = Clipboard.GetText();
            //for (int x = 0; x < 320; x++)
            //{
            //    for (int y = 0; y < 240; y++)
            //    {
            //        var pixelColorString = hexString.Substring(4*x + 4*320*y, 4);

            //        var pix = Convert.ToUInt16(pixelColorString, 16);


            //        //* For PixelFormat.Format16bppRgb565
            //        //var r = BitConverter.GetBytes((UInt16)((pix & 0xF800) >> 11));

            //        int red = ((pix >> 11)*527 + 23) >> 6;
            //        int green = (((pix >> 5) & 0x003F)*259 + 33) >> 6;
            //        int blue = ((pix & 0x001F)*527 + 23) >> 6;

            //        bitmap.SetPixel(x, y, Color.FromArgb(red, green, blue));
            //    }
            //}

            loadImage(Clipboard.GetText());

        }

        private void loadImage(string hexString)
        {
            Console.WriteLine(hexString);
            lblFailed.Visible = false;
            Bitmap bitmap;
            if (isLandscape)
            {
                bitmap = new Bitmap(320, 240, PixelFormat.Format24bppRgb);
            }
            else
            {
                bitmap = new Bitmap(240, 320, PixelFormat.Format24bppRgb);
            }

            try
            {

                //for (int x = 0; x < 320; x++)
                //{
                //    for (int y = 0; y < 240; y++)
                //    {
                //        var pixelColorString = hexString.Substring(6 * (x + 320 * y), 6);

                //        var r = Convert.ToByte(pixelColorString.Substring(0, 2), 16);
                //        var g = Convert.ToByte(pixelColorString.Substring(2, 2), 16);
                //        var b = Convert.ToByte(pixelColorString.Substring(4, 2), 16);

                //        //* For PixelFormat.Format16bppRgb565
                //        //var r = BitConverter.GetBytes((UInt16)((pix & 0xF800) >> 11));

                //        //int red = ((pix >> 11) * 527 + 23) >> 6;
                //        //int green = (((pix >> 5) & 0x003F) * 259 + 33) >> 6;
                //        //int blue = ((pix & 0x001F) * 527 + 23) >> 6;

                //        bitmap.SetPixel(x, y, Color.FromArgb(r, g, b));
                //    }
                //}
                int x = 0, y = 0;
                var totalImageDataSize = Convert.ToUInt32(hexString.Substring(hexString.Length - 8, 8), 16);
                if (hexString.Length - 8 == totalImageDataSize)
                {
                    for (int i = 0; i < totalImageDataSize; i += 10) // 6 bytes for color, 4 bytes for length
                    {
                        var pixelColorString = hexString.Substring(i, 6);
                        var r = Convert.ToByte(pixelColorString.Substring(0, 2), 16);
                        var g = Convert.ToByte(pixelColorString.Substring(2, 2), 16);
                        var b = Convert.ToByte(pixelColorString.Substring(4, 2), 16);

                        var pixelCountString = hexString.Substring(i + 6, 4);
                        var pixelCount = Convert.ToUInt16(pixelCountString, 16);

                        for (int p = 0; p < pixelCount; p++)
                        {
                            bitmap.SetPixel(x, y, Color.FromArgb(r, g, b));
                            x++;
                            if (x > (isLandscape ? 319 : 239))
                            {
                                x = 0;
                                y++;
                            }
                        }
                    }

                    pictureBox.Image = bitmap;

                    btnSaveAs.Visible = true;
                }
                else
                {
                    lblFailed.Text = "Image Data length mismatch.";
                    pictureBox.Image = null;
                    lblFailed.Visible = true;
                    btnSaveAs.Visible = false;
                }
            }
            catch (Exception ex)
            {
                lblFailed.Text = "Failed to load the image.\nTry increasing ILI9341_SPI_CLKDIVIDER or\nchange to NORMAL or EXTENDED SPI mode.";
                textBox1.AppendText(ex.Message + "\r\n");
                pictureBox.Image = null;
                lblFailed.Visible = true;
                btnSaveAs.Visible = false;
            }
        }

        private void btnSaveAs_Click(object sender, EventArgs e)
        {
            saveFileDialog.FileName = Path.GetFileNameWithoutExtension(saveFileDialog.FileName);
            var result = saveFileDialog.ShowDialog();
            if (result == DialogResult.OK)
            {
                ImageFormat imageFormat = ImageFormat.Bmp;
                switch (Path.GetExtension(saveFileDialog.FileName).ToLower())
                {
                    case ".jpg":
                        imageFormat = ImageFormat.Jpeg;
                        break;
                    case ".png":
                        imageFormat = ImageFormat.Png;
                        break;
                    case ".gif":
                        imageFormat = ImageFormat.Gif;
                        break;
                }

                pictureBox.Image.Save(saveFileDialog.FileName, imageFormat);
            }

        }

        private void Form1_Load(object sender, EventArgs e)
        {
            cbSerialPorts.Items.AddRange(SerialPort.GetPortNames());
            cbSerialPorts.Text = Properties.Settings.Default.Port;
            cbBaudRates.Text = Properties.Settings.Default.BaudRate;
        }

        private void btnConnect_Click(object sender, EventArgs e)
        {
            if (serialPort1.IsOpen)
            {
                serialPort1.Close();
                btnConnect.Text = "Connect";
                textBox1.AppendText(serialPort1.PortName + " closed\r\n");
            }
            else
            {
                try
                {
                    serialPort1.PortName = cbSerialPorts.Text;
                    serialPort1.BaudRate = Int32.Parse(cbBaudRates.Text);
                    serialPort1.DataBits = 8;
                    serialPort1.Parity = Parity.None;
                    serialPort1.StopBits = StopBits.One;
                    serialPort1.DtrEnable = true;
                    serialPort1.Open();
                    textBox1.AppendText(serialPort1.PortName + " opened\r\n");
                    btnConnect.Text = "Close";
                }
                catch (Exception ex)
                {
                    textBox1.AppendText(ex.Message + "\r\n");
                }

            }
        }

        private void serialPort1_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            try{
                string data = serialPort1.ReadLine();
                if (loadImageFromData && data != string.Empty)
                {
                    loadImageFromData = false;
                    data = data.TrimEnd('\r');
                    
                    this.Invoke(new Action<string>(d => { loadImage(d); }), new object[] { data });
                }
                else if (data.Contains("= PIXEL DATA START ="))
                {
                    this.Invoke(new Action<string>(d => { textBox1.AppendText("Loading Image...\r\n"); textBox1.ScrollToCaret(); textBox1.Refresh(); }), new object[] { data });
                    loadImageFromData = true;
                }
                else if (data.Contains("= PIXEL DATA END ="))
                { }
                else
                    this.Invoke(new Action<string>(d => { textBox1.AppendText(d + "\r\n"); textBox1.ScrollToCaret(); }), new object[] { data });
            }
            catch
            {}
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            Properties.Settings.Default.Port = cbSerialPorts.Text;
            Properties.Settings.Default.BaudRate = cbBaudRates.Text;
            Properties.Settings.Default.Save();
        }

        private void rbLandscape_CheckedChanged(object sender, EventArgs e)
        {
            isLandscape = true;
        }

        private void rbPortrait_CheckedChanged(object sender, EventArgs e)
        {
            isLandscape = false;
        }
    }
}
