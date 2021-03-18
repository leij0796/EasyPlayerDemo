using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Interop;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using WindowsFormsApp1;

namespace WpfApp1
{
    /// <summary>
    /// MainWindow.xaml 的交互逻辑
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
            Loaded += MainWindow_Loaded;
        }

        private void MainWindow_Loaded(object sender, RoutedEventArgs e)
        {
            PlayerSdk.EPInit();
            //IntPtr hwnd = ((HwndSource)PresentationSource.FromVisual(VideoPictureBox)).Handle;
            HwndSource hs1 = (HwndSource)PresentationSource.FromDependencyObject(gg1);
            HwndSource hs2 = (HwndSource)PresentationSource.FromDependencyObject(gg2);
            IntPtr ip = hs2.Handle;
            //IntPtr ip1 = hs1.Handle;
            int channelID = PlayerSdk.EasyPlayer_OpenStream("rtsp://172.16.21.209:554/1", VideoPictureBox.Handle, PlayerSdk.RENDER_FORMAT.DISPLAY_FORMAT_RGB24_GDI, 1, "", "", PlayerSdk.EPcallBack, IntPtr.Zero, false);
            if (channelID > 0)
            {
                PlayerSdk.EasyPlayer_SetFrameCache(channelID, 3);
            }
        }

        private void Gg2_OnMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            var m = Canvas.GetZIndex(gg2);
            var m2 = Canvas.GetZIndex(gg1);
            Canvas.SetZIndex(gg2, 10000);
        }
    }
}
