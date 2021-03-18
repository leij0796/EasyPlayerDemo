using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using WindowsFormsApp1;

namespace WpfApp1
{
    public partial class UserControl2 : UserControl
    {
        public UserControl2()
        {
            InitializeComponent();
            Load += UserControl1_Load;

            //int channelID = PlayerSdk.EasyPlayer_OpenStream("rtsp://admin:itc123456@172.16.21.80:554/h264/ch1/main/av_stream", panel1.Handle, PlayerSdk.RENDER_FORMAT.DISPLAY_FORMAT_RGB24_GDI, 1, "admin", "itc123456", PlayerSdk.EPcallBack, IntPtr.Zero, false);
            //if (channelID > 0)
            //{
            //    PlayerSdk.EasyPlayer_SetFrameCache(channelID, 3);
            //}
        }

        private void UserControl1_Load(object sender, EventArgs e)
        {
            //HwndSource hs = (HwndSource)PresentationSource.FromDependencyObject(asd);
            //IntPtr ip = hs.Handle;
            PlayerSdk.EPInit();
            int channelID = PlayerSdk.EasyPlayer_OpenStream("rtsp://admin:itc123456@172.16.21.80:554/h264/ch1/main/av_stream", panel1.Handle, PlayerSdk.RENDER_FORMAT.DISPLAY_FORMAT_RGB24_GDI, 1, "admin", "itc123456", PlayerSdk.EPcallBack, IntPtr.Zero, false);
            if (channelID > 0)
            {
                PlayerSdk.EasyPlayer_SetFrameCache(channelID, 3);
            }
        }
    }
}
