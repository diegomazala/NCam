
using UnityEngine;
using System.Collections;
using System.Runtime.InteropServices;
using System.Xml;


public class LensEncoderData
{
    public int[] data = null;
    public GCHandle Handle;

    public LensEncoderData()
    {
        data = new int[3]{0, 0, 0};
        Handle = GCHandle.Alloc(data, GCHandleType.Pinned);
    }

    public LensEncoderData(int[] int_array_3)
    {
        data = int_array_3;
        Handle = GCHandle.Alloc(data, GCHandleType.Pinned);
    }

    ~LensEncoderData()
    {
        Handle.Free();
    }
        
}




public class LensEncoder : MonoBehaviour
{
    public int port = 2;
    public LensEncoderData encoderData = new LensEncoderData();

    private class Plugin
    {
        [DllImport("LensEncoder")]
        public static extern bool LensEncoderConnect(int port, bool multihread);
        [DllImport("LensEncoder")]
        public static extern bool LensEncoderIsConnected();
        [DllImport("LensEncoder")]
        public static extern void LensEncoderDisconnect();
        [DllImport("LensEncoder")]
        public static extern void LensEncoderUpdate();
        [DllImport("LensEncoder")]
        public static extern bool LensEncoderGetData(System.IntPtr intArray3);
    }

    public int Zoom
    {
        get { return encoderData.data[0]; }
        set { encoderData.data[0] = value; }
    }

    public int Focus
    {
        get { return encoderData.data[1]; }
        set { encoderData.data[1] = value; }
    }

    public int Iris
    {
        get { return encoderData.data[2]; }
        set { encoderData.data[2] = value; }
    }


    public void Connect()
    {
        StartCoroutine(ConnectToEncoder());
    }
        

    public bool IsConnected()
    {
        return Plugin.LensEncoderIsConnected();
    }


    public void Disconnect()
    {
        Plugin.LensEncoderDisconnect();
    }


    private IEnumerator ConnectToEncoder()
    {
        Disconnect();

        yield return new WaitForEndOfFrame();

        bool success = false;

        success = Plugin.LensEncoderConnect(port, true);

        if (success)
        {
            //if (SystemInfo.graphicsDeviceVersion.Contains("OpenGL"))
                StartCoroutine("LensEncoderUpdateAtEndOfFrames");
        }
        else
        {
            Debug.LogError("Could not connect to Lens Encoder");
        }

        yield return null;
    }


    private IEnumerator LensEncoderUpdateAtEndOfFrames()
    {
        yield return new WaitForEndOfFrame();

        while (true)
        {
            // Wait until all frame rendering is done
            yield return new WaitForEndOfFrame();

            if (!IsConnected())
            {
                yield return null;
            }
            else
            {
                Plugin.LensEncoderGetData(encoderData.Handle.AddrOfPinnedObject());
            }


            yield return null;
        }
    }


    static public string xmlConfigFolder
    {
        get
        {
#if UNITY_EDITOR
            System.IO.DirectoryInfo mainFolderPath = new System.IO.DirectoryInfo(Application.dataPath + "/");
#else
            System.IO.DirectoryInfo mainFolderPath = new System.IO.DirectoryInfo(Application.dataPath + "/../");
#endif
            return mainFolderPath.FullName + @"Config/Sensor/";
        }
    }


    void OnEnable()
    {

#if !UNITY_EDITOR
        // If the xml configuration file does not exists, create it
        if (!ReadXml(xmlConfigFolder + @"LensEncoder.xml"))
        {
            System.IO.DirectoryInfo sensorDir = new System.IO.DirectoryInfo(xmlConfigFolder);
            if (!sensorDir.Exists)
            {
                sensorDir.Create();
            }
            WriteXml(xmlConfigFolder + @"LensEncoder.xml");
        }
#endif
    }


    void OnDisable()
    {
        Disconnect();
    }


    void Update()
    {
        if (IsConnected())
        {
            Plugin.LensEncoderUpdate();
        }
    }



    public bool ReadXml(string file_path)
    {
        XmlDocument xmlDoc = new XmlDocument();

        try
        {
            xmlDoc.Load(file_path);

            XmlNodeList docXml = xmlDoc.GetElementsByTagName("LensEncoder");
            XmlNode ncamXml = docXml[0];

            if (ncamXml != null)
            {
                bool isEnabled = false;
                XmlIO.Read(ncamXml.ChildNodes[0], ref isEnabled);
                enabled = isEnabled;
                XmlIO.Read(ncamXml.ChildNodes[1], ref port);
            }
        }
        catch (System.Exception e)
        {
            Debug.LogError(e.Message);
            return false;
        }
        return true;
    }


    public bool WriteXml(string file_path)
    {
        XmlDocument xmlDoc = new XmlDocument();

        XmlDeclaration xmlDec = xmlDoc.CreateXmlDeclaration("1.0", null, null);
        xmlDoc.AppendChild(xmlDec);

        XmlElement ncamXml = xmlDoc.CreateElement("LensEncoder");

        XmlIO.Write(ncamXml, enabled, "Enabled");
        XmlIO.Write(ncamXml, port, "Port");

        xmlDoc.AppendChild(ncamXml);

        try
        {
            xmlDoc.Save(file_path);
        }
        catch (System.Exception e)
        {
            Debug.LogError(e.Message);
            return false;
        }
        return true;
    }
}
