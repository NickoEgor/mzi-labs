using CommonClasses;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Imaging;
using System.Linq;
using System.Text;
using CommonClasses;

using System.Threading.Tasks;

namespace Lab8
{
    public class KochZhao
    {
        private Bitmap _picture;

        private int Wn => _picture.Width - (_picture.Width % 8);

        private int Wm => _picture.Height - (_picture.Height % 8);

        private readonly List<List<List<int>>> _tables = new List<List<List<int>>>();
        private readonly List<List<List<double>>> _dkp = new List<List<List<double>>>();

        private readonly double P = 3;
        private readonly int u1 = 7;
        private readonly int v1 = 7;
        private readonly int u2 = 7;
        private readonly int v2 = 6;

        public KochZhao(string path)
        {
            _picture = new Bitmap(path);
        }

        public void EncodeMessage(string message, string path)
        {
            BreakPicture();

            foreach (var table in _tables)
                _dkp.Add(DkpFunc(table));

            SetMessage(message);

            for (int i = 0; i < _tables.Count; ++i)
                _tables[i] = OdkpFunc(_dkp[i]);

            BuildPicture();

            _picture.Save(path, ImageFormat.Jpeg);
            _picture.Dispose();
        }

        public string GetMessage(string path)
        {
            _picture = new Bitmap(path);

            BreakPicture();

            foreach (var table in _tables)
                _dkp.Add(DkpFunc(table));

            _picture.Dispose();

            return GetMessage();
        }

        private string GetMessage()
        {
            var paths = new List<string>();

            foreach (var d in _dkp)
            {
                var abs1 = Math.Abs(d[u1][v1]);
                var abs2 = Math.Abs(d[u2][v2]);

                paths.Add(abs1 > abs2 ? "0" : "1");
            }

            var basis = string.Join("", paths).ToUtf();
            var bytes = new List<byte>();

            for (int i = 0; i < basis.Length - (basis.Length % 8); i += 8)
                bytes.Add((byte)Convert.ToInt32(string.Join("", basis.Substring(i, 8).Reverse()), 2));

            return Encoding.UTF8.GetString(bytes.ToArray()).Trim();
        }

        private void SetMessage(string message)
        {
            if (message.Length * 8 > _tables.Count)
                throw new Exception("Message is too long for selected picture!");

            var str = new StringBuilder(message);

            for (int i = str.Length; i < _tables.Count / 8; ++i)
                str.Append(' ');

            var bytes = new BitArray(Encoding.UTF8.GetBytes(str.ToString()));
            bytes.Normalize();

            for (int i = 0; i < Math.Min(_dkp.Count, bytes.Length); ++i)
            {
                double k1 = _dkp[i][u1][v1];
                double k2 = _dkp[i][u2][v2];

                double abs1 = Math.Abs(k1);
                double abs2 = Math.Abs(k2);

                double sign1 = k1 >= 0 ? 1 : -1;
                double sign2 = k2 >= 0 ? 1 : -1;

                if (!bytes[i] && abs1 - abs2 <= P)
                    abs1 = abs2 + P;

                if (bytes[i] && abs1 - abs2 >= -P)
                    abs2 = abs1 + P;

                _dkp[i][u1][v1] = sign1 * abs1;
                _dkp[i][u2][v2] = sign2 * abs2;
            }
        }

        private void BreakPicture()
        {
            _tables.Clear();
            _dkp.Clear();

            var sqr = new List<List<int>>();

            for (int ii = 0; ii < Wn; ii += 8)
                for (int jj = 0; jj < Wm; jj += 8)
                {
                    for (int i = ii; i < ii + 8; ++i)
                    {
                        var row = new List<int>();

                        for (int j = jj; j < jj + 8; ++j)
                            row.Add(_picture.GetPixel(i, j).B);

                        sqr.Add(row);
                    }

                    _tables.Add(sqr);
                    sqr = new List<List<int>>();
                }
        }

        private void BuildPicture()
        {
            var position = 0;

            int min = int.MaxValue, max = int.MinValue;

            for (int ii = 0; ii < Wn; ii += 8)
                for (int jj = 0; jj < Wm; jj += 8)
                {
                    for (int i = ii; i < ii + 8; ++i)
                        for (int j = jj; j < jj + 8; ++j)
                        {
                            var val = _tables[position][i - ii][j - jj];
                            min = Math.Min(min, val);
                            max = Math.Max(max, val);
                        }

                    position++;
                }

            position = 0;

            for (int ii = 0; ii < Wn; ii += 8)
                for (int jj = 0; jj < Wm; jj += 8)
                {
                    for (int i = ii; i < ii + 8; ++i)
                        for (int j = jj; j < jj + 8; ++j)
                        {
                            var pixel = _picture.GetPixel(i, j);

                            var pos = 255 * (_tables[position][i - ii][j - jj] + Math.Abs(min)) / (max + Math.Abs(min));

                            _picture.SetPixel(i, j, Color.FromArgb(pixel.A, pixel.R, pixel.G, pos));
                        }

                    position++;
                }
        }

        private List<List<double>> DkpFunc(List<List<int>> matrix)
        {
            var ans = new List<List<double>>();
            int n = matrix.Count();

            for (int i = 0; i < n; ++i)
                ans.Add(new List<double>(Enumerable.Repeat(0.0, n)));

            for (int v = 0; v < n; v++)
                for (int u = 0; u < n; u++)
                {
                    double V = v == 0 ? 1.0 / Math.Sqrt(2) : 1.0;
                    double U = u == 0 ? 1.0 / Math.Sqrt(2) : 1.0;

                    double temp = 0;

                    for (int i = 0; i < n; i++)
                        for (int j = 0; j < n; j++)
                            temp += matrix[i][j] * Math.Cos(Math.PI * v * (2 * i + 1) / (2 * n)) *
                                Math.Cos(Math.PI * u * (2 * j + 1) / (2 * n));

                    ans[v][u] = U * V * temp / Math.Sqrt(2 * n);
                }

            return ans;
        }

        private List<List<int>> OdkpFunc(List<List<double>> matrix)
        {
            var ans = new List<List<int>>();
            int n = matrix.Count();

            for (int i = 0; i < n; ++i)
                ans.Add(new List<int>(Enumerable.Repeat(0, n)));

            for (int v = 0; v < n; v++)
                for (int u = 0; u < n; u++)
                {
                    double temp = 0;

                    for (int i = 0; i < n; i++)
                        for (int j = 0; j < n; j++)
                        {
                            double V = i == 0 ? 1.0 / Math.Sqrt(2) : 1.0;
                            double U = j == 0 ? 1.0 / Math.Sqrt(2) : 1.0;

                            temp += U * V * matrix[i][j] * Math.Cos(Math.PI * i * (2 * v + 1) / (2 * n)) *
                                Math.Cos(Math.PI * j * (2 * u + 1) / (2 * n));
                        }

                    ans[v][u] = (int)Math.Round(temp / Math.Sqrt(2 * n));
                }

            return ans;
        }

        private List<List<double>> Normalize(List<List<double>> matrix)
        {
            double min = double.MaxValue, max = double.MinValue;

            for (int i = 0; i < matrix.Count; ++i)
                for (int j = 0; j < matrix[0].Count; ++j)
                {
                    max = Math.Max(max, matrix[i][j]);
                    min = Math.Min(min, matrix[i][j]);
                }

            for (int i = 0; i < matrix.Count; i++)
                for (int j = 0; j < matrix[0].Count; j++)
                    matrix[i][j] = 255 * (matrix[i][j] + Math.Abs(min)) / (max + Math.Abs(min));

            return matrix;
        }

        private List<List<int>> Normalize(List<List<int>> matrix)
        {
            double min = int.MaxValue, max = int.MinValue;

            for (int i = 0; i < matrix.Count; ++i)
                for (int j = 0; j < matrix[0].Count; ++j)
                {
                    max = Math.Max(max, matrix[i][j]);
                    min = Math.Min(min, matrix[i][j]);
                }

            //if (max > 255 || min < 0)
            for (int i = 0; i < matrix.Count; i++)
                for (int j = 0; j < matrix[0].Count; j++)
                    matrix[i][j] = (int)(255 * (matrix[i][j] + Math.Abs(min)) / (max + Math.Abs(min)));

            return matrix;
        }
    }
}
