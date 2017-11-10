using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace LocalServer
{
    public class ClaseMain
    {
        public static void StartListening()
        {
            // DATA INICIALIZATION
            byte[] bytes;   // Data buffer for incoming data.
            IPHostEntry ipHostInfo;
            IPAddress ipAddress;
        }
        public static int Main(string[] args)
        {
            //Your program starts here...
            // https://msdn.microsoft.com/es-es/library/6y0e13d3(v=vs.110).aspx
            StartListening();
            return 0;
        }
    }
}
