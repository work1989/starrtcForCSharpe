using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;

using System.Windows.Forms;

namespace WindowsFormsApplication1
{
    public partial class indexForm : Form
    {
        [DllImport("ypVoip.Dll", EntryPoint = "YPLogin", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern bool YPLogin(string userID);

        [DllImport("ypVoip.Dll", EntryPoint = "VoipCallPerson", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern int VoipCallPerson(string userID,ref byte fan);

        [DllImport("ypVoip.Dll", EntryPoint = "DeleteVoip", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]

        public static extern int DeleteVoip();

        public indexForm()
        {
            InitializeComponent();
        }
        bool isInit = false;
        private void button1_Click(object sender, EventArgs e)
        {
            if (!isInit) {
                MessageBox.Show("请启动视频服务");
                return;
            }
            if (String.IsNullOrEmpty(this.textBox1.Text)) {
                MessageBox.Show("请输入账号");
                return;
            }
            byte[] s = new byte[1024];
            VoipCallPerson(this.textBox1.Text, ref s[0]);
            //string strGet = System.Text.Encoding.Default.GetString(s, 0, s.Length); //将字节数组转换为字符串
            //MessageBox.Show(strGet + "");
        }

        private void eventLog1_EntryWritten(object sender, System.Diagnostics.EntryWrittenEventArgs e)
        {

        }

        private void Form1_Load(object sender, EventArgs e)
        {

        }

        //private void button1_Click_1(object sender, EventArgs e)
        //{
        //    if (String.IsNullOrEmpty(this.textBox1.Text))
        //    {
        //        MessageBox.Show("请输入账号");
        //        return;
        //    }
        //    byte[] s = new byte[1024];
        //    VoipCallPerson(this.textBox1.Text, ref s[0]);
        //}

        private void button1_Click_2(object sender, EventArgs e)
        {
            string _selfNo = this.selfNo.Text;
            if (string.IsNullOrEmpty(_selfNo))
            {
                MessageBox.Show("请初始化账号");
                return;
            }
            isInit = true;
            YPLogin(_selfNo);
            this.button1.Enabled = false;
        }

        private void indexForm_FormClosed(object sender, FormClosedEventArgs e)
        {
             DeleteVoip();
        }
    }
}
