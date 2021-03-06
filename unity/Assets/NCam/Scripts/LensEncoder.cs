
using UnityEngine;
using System.Collections;
using System.Runtime.InteropServices;
using System.Xml;

[System.Serializable]
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
    public bool multithreadEncoder = true;
    public int port = 2;
    

    [SerializeField] 
    private LensEncoderData encoderData = new LensEncoderData();

    private float max = 65535;

    public enum LensEncoderRenderEvent
    {
        Initialize,
        Update,
        Uninitialize
    };



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
        [DllImport("LensEncoder")]
        public static extern System.IntPtr GetLensEncoderRenderEventFunc();
    }


    public int ZoomMapped
    {
        get { return encoderData.data[0]; }
        set { encoderData.data[0] = value; }
    }

    public int FocusMapped
    {
        get { return encoderData.data[1]; }
        set { encoderData.data[1] = value; }
    }

    public int IrisMapped
    {
        get { return encoderData.data[2]; }
        set { encoderData.data[2] = value; }
    }

    public float ZoomNormalized
    {
        get { return encoderData.data[0] / max; }
        set { encoderData.data[0] = (int)(value * max); }
    }

    public float FocusNormalized
    {
        get { return encoderData.data[1] / max; }
        set { encoderData.data[1] = (int)(value * max); }
    }

    public float IrisNormalized
    {
        get { return encoderData.data[2] / max; }
        set { encoderData.data[2] = (int)(value * max); }
    }


    public bool Connect()
    {
        if (Plugin.LensEncoderConnect(port, multithreadEncoder))
        {
            StartCoroutine(GetEncoderData());
            return true;
        }
        return false;
    }
        

    public bool IsConnected()
    {
        return Plugin.LensEncoderIsConnected();
    }


    public void Disconnect()
    {
        StopCoroutine(GetEncoderData());
        Plugin.LensEncoderDisconnect();
    }



    private IEnumerator GetEncoderData()
    {
        yield return new WaitForEndOfFrame();

        while (true)
        {
            // Wait until all frame rendering is done
            yield return new WaitForEndOfFrame();
            
            GL.IssuePluginEvent(Plugin.GetLensEncoderRenderEventFunc(), (int)LensEncoderRenderEvent.Update);
            Plugin.LensEncoderGetData(encoderData.Handle.AddrOfPinnedObject());
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
            return mainFolderPath.FullName + @"Config/Lens/";
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
      enabled = Connect();
    }


    void OnDisable()
    {
        Disconnect();
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
                XmlIO.Read(ncamXml.ChildNodes[2], ref multithreadEncoder);
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
        XmlIO.Write(ncamXml, multithreadEncoder, "Multithread");

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
