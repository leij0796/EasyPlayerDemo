using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace WindowsFormsApp1
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
            Load += Form1_Load;
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            PlayerSdk.EPInit();
            var p1 = panel1.Handle;
            //var p2 = panel2.Handle;
            int channelID = PlayerSdk.EasyPlayer_OpenStream("rtsp://172.16.21.209:554/1", panel1.Handle, PlayerSdk.RENDER_FORMAT.DISPLAY_FORMAT_RGB24_GDI, 1, "admin", "itc123456", PlayerSdk.EPcallBack, IntPtr.Zero, false);
            if (channelID > 0)
            {
                PlayerSdk.EasyPlayer_SetFrameCache(channelID, 3);
            }
        }
    }
}
