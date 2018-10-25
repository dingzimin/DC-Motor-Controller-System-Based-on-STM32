using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Threading;
using System.Windows;
using System.Windows.Input;
using LiveCharts;
using LiveCharts.Defaults;
using LiveCharts.Wpf;

namespace WpfApp {
    public partial class MainWindow : Window {
        private SerialPort port; // 串口对象
        private readonly LineSeries line; // 速度曲线对象
        private int i = 0; // 时间计数
        private double lspeed = 0; // 上一次指数加权平均的速度

        public MainWindow() {
            InitializeComponent();
            InitPort();
            line = new LineSeries();
            line.PointGeometry = null;
            line.Values = new ChartValues<ObservablePoint>();
            ChartMain.Series = new SeriesCollection {line};
            SliderSpeed.Focus();
        }
        /* 初始化串口传输 */
        private void InitPort() {
            var n = SerialPort.GetPortNames();
            if (n.Length == 0)
                return;
            // 实例化串口对象，波特率 1Mbps
            port = new SerialPort(n[0], 1000000, Parity.None, 8, StopBits.One);
            port.Open(); // 打开串口
            bool sta = true;
            new Thread(() => { // 在新的线程中接收数据
                try {
                    while (true) {
                        int byten = port.BytesToRead & 0x7FFFFFFE;
                        byte[] data = new byte[byten];
                        port.Read(data, 0, byten); // 读取数据
                        if (!sta)
                            continue;
                        var list = new List<ObservablePoint>(byten >> 1);
                        // 计算散点坐标
                        for (int j = 0; j < byten; j += 2) {
                            var val = (short) ((data[j] << 8) | data[j + 1]);
                            if (val > 4000 || val < -4000)
                                continue;
                            var speed = (double) val * 50 * 3 / 52;
                            // 计算加权平均速度
                            lspeed = 0.875 * lspeed + 0.125 * speed;
                            list.Add(new ObservablePoint(i / 50.0, speed));
                            i++;
                        }
                        Dispatcher.Invoke(() => { // 在 UI 线程刷新曲线
                            var count = line.Values.Count - 500;
                            for (int j = 0; j < count; j++)
                                line.Values.RemoveAt(0);
                            line.Values.AddRange(list);
                            TextSpeed.Text = $"{Math.Round(lspeed)} rpm";
                        });
                        Thread.Sleep(100); // 线程睡眠 100ms
                    }
                } catch (Exception e) { }
            }).Start();
            ButtonStop.MouseDown += (sender, args) => { // 清空曲线
                line.Values.Clear();
                lspeed = 0;
                i = 0;
            };
            ButtonClear.MouseDown += (sender, args) => { // 开始和停止刷新速度曲线
                sta = !sta;
                ButtonClear.Text = sta ? "停止" : "开始";
            };
        }
        /* 速度滑动条改变，发送新的给定速度到下位机 */
        private void SliderSpeed_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e) {
            var s = Math.Round(SliderSpeed.Value / 3);
            TextSetSpeed.Text = $"{(int) Math.Round(SliderSpeed.Value)}";
            var b = (sbyte) s;
            byte[] a = {(byte) b};
            port.Write(a, 0, 1);
        }
        /* 速度的值改变，发送新的给定速度到下位机 */
        private void TextSetSpeed_KeyDown(object sender, KeyEventArgs e) {
            if (e.Key == Key.Enter) {
                int s;
                if (int.TryParse(TextSetSpeed.Text, out s)) {
                    if (s >= -300 && s <= 300)  {
                        byte[] a = { (byte)(s + 3 / 3) };
                        port.Write(a, 0, 1);
                        SliderSpeed.Value = s;
                        return;
                    }
                }
                TextSetSpeed.Text = $"{(int) Math.Round(SliderSpeed.Value)}";
            }
        }
        /* 发送新的 PI 参数到下位机，仅用于调试 */
        private void ButtonSendPI_Click(object sender, RoutedEventArgs e) {
            float kkp, kki;
            if (float.TryParse(TextKp.Text, out kkp) &&
                float.TryParse(TextKi.Text, out kki)) {
                sendPI(kkp, kki);  
            }
        }
        /* 通过串口发送 PI 参数 */
        private unsafe void sendPI(float kp, float ki) {
            byte* pp = (byte*) &kp;
            byte* pi = (byte*) &ki;
            byte[] data = new byte[9] {
                0x7F,
                pp[0], pp[1], pp[2], pp[3],
                pi[0], pi[1], pi[2], pi[3]
            };
            port.Write(data, 0, 9);
        }
    }
}