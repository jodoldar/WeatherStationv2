using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;
using System.Timers;

namespace WeatherService
{
    class Program
    {

        public static string data = null;

        public static void StartListening()
        {
            // DATA INICIALIZATION
            byte[] bytes;   // Data buffer for incoming data.
            IPHostEntry ipHostInfo;
            IPAddress ipAddress;
            IPEndPoint localEndPoint;

            bytes = new Byte[1024];
            ipHostInfo = Dns.GetHostEntry(Dns.GetHostName());
            ipAddress = null;
           
            foreach(IPAddress ipAux in ipHostInfo.AddressList)
            {
                IPAddress aux = ipAux.MapToIPv4();
                if (aux.ToString().StartsWith("192.168."))
                {
                    ipAddress = ipAux;
                }
            }

            if(ipAddress == null)
            {
                Console.WriteLine("No hay una interfaz local disponible");
                return;
            }
            localEndPoint = new IPEndPoint(ipAddress, 11000);
            Console.WriteLine("Connected in: {0}", localEndPoint);

            // Socket creation
            Socket listener = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

            try
            {
                listener.Bind(localEndPoint);
                listener.Listen(10);

                // Socket now listening to connections
                while (true)
                {
                    Console.WriteLine("Waiting for a connection...");
                    Socket handler = listener.Accept();
                    data = null;
                    Console.WriteLine("Connection received");

                    while (true)
                    {
                        bytes = new byte[1024];
                        int bytesRec = handler.Receive(bytes);
                        data += Encoding.ASCII.GetString(bytes, 0, bytesRec);
                        if (data.IndexOf("<EOF>") > -1)
                        {
                            break;
                        }
                    }

                    Console.WriteLine("Text received: {0}", data);

                    byte[] msg = Encoding.ASCII.GetBytes(data);
                    handler.Send(msg);
                    handler.Shutdown(SocketShutdown.Both);
                    handler.Close();
                }
            }catch(Exception e)
            {
                Console.WriteLine(e.ToString());
            }

            Console.WriteLine("\nPress ENTER to continue...");
            Console.Read();
        }

        static public void TestDatabase()
        {
            var db = new StoringContext();
            var measure = new Measurement (20.0, 50.0, 1013, 5.0, 180, 10.0, 185, DateTime.Now );
            db.MeasurementSet.Add(measure);
            db.SaveChanges();
        }

        static int Main(string[] args)
        {
            //Your program starts here...
            // https://msdn.microsoft.com/es-es/library/6y0e13d3(v=vs.110).aspx

            TestDatabase();
            StartListening();
            return 0;
        }
    }
}

