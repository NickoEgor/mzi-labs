using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Lab8
{
    class Program
    {
        static void Main(string[] args)
        {
            var pic = Path.Combine(Environment.CurrentDirectory, "pic.jpg");
            var encPic = Path.Combine(Environment.CurrentDirectory, "enc_pic.jpg");
            var message = "Super secret message";

            var algo = new KochZhao(originPath);
            Console.WriteLine($"Encoded message: {message}");
            algo.EncryptMessage(message, convertPath);
            Console.WriteLine($"Decoded message: {algo.GetMessage(convertPath)}");
        }
    }
}
