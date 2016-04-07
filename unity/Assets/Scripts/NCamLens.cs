
using UnityEngine;
using System.Collections;
using System.Runtime.InteropServices;
using System.Xml;


[RequireComponent(typeof(NCamLensTableGUI))]
public class NCamLens : MonoBehaviour
{
    public string ipAddress = "127.0.0.1";
    public int port = 38860; //3003;

    
    public NCamOptical ncamOptical = null;

    private NCamEncoder ncamEncoder = null;

    [HideInInspector]
    public RenderTexture DistortMap = null;
    [HideInInspector]
    public Vector2 DistortMapSize;


    private bool connecting = false;
    private bool updateData = true;


    #region GUI Variables
    private GUIWindow guiWindow = null;
    public bool showGUI = false;
    private bool showEncoderGUI = false;
    #endregion


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


    public NCamOptical Optical
    {
        get { return ncamOptical; }
    }



    void Awake()
    {        
        ncamOptical = new NCamOptical();
        ncamEncoder = new NCamEncoder();
        DistortMapSize = Vector2.one;
    }





    void OnEnable()
    {
        NCamPlugin.NCamCreate();


#if !UNITY_EDITOR
        // If the xml configuration file does not exists, create it
        if (!ReadXml(xmlConfigFolder + @"NCam.xml"))
        {
            System.IO.DirectoryInfo sensorDir = new System.IO.DirectoryInfo(xmlConfigFolder);
            if (!sensorDir.Exists)
            {
                sensorDir.Create();
            }
        }
#endif

        StartCoroutine(OpenNCam());
    }


    void OnDisable()
    {
        CloseNCam();


        NCamPlugin.NCamDestroy();
    }


    private IEnumerator OpenNCam()
    {
        if (connecting)
        {
            yield return null;
        }

        connecting = true;

        NCamPlugin.NCamResetError();
        NCamPlugin.NCamSetPacketType(true, true, true);

        yield return new WaitForEndOfFrame();


        NCamPlugin.NCamSetIpAddress(ipAddress, port);
        bool success = NCamPlugin.NCamOpen();

        if (success)
        {
            if (SystemInfo.graphicsDeviceVersion.Contains("OpenGL"))
                StartCoroutine("NCamUpdateAtEndOfFrames");
        }
        else
        {
            Debug.LogError("Could not open NCam");
        }

        yield return new WaitForSeconds(1.0f);
        connecting = false;
        yield return null;
    }



    void CloseNCam()
    {
        StopCoroutine("NCamUpdateAtEndOfFrames");

        NCamPlugin.NCamClose();
    }


    private IEnumerator NCamUpdateAtEndOfFrames()
    {
        yield return new WaitForEndOfFrame();

        if (DistortMap != null)
            NCamPlugin.NCamSetDistortMapPtr(DistortMap.GetNativeTexturePtr());

        while (true)
        {
            // Wait until all frame rendering is done
            yield return new WaitForEndOfFrame();

            if (!NCamPlugin.NCamIsOpen())
            {
                yield return null;
            }

            if (DistortMap == null)
            {
                int w = NCamPlugin.NCamDistortMapWidth();
                int h = NCamPlugin.NCamDistortMapHeight();

                DistortMapSize = new Vector2(w, h);

                if (w > 1 && h > 1) // otherwise the reading has not been initialized yet
                {
                    DistortMap = new RenderTexture(w, h, 8, RenderTextureFormat.RGFloat);
                    DistortMap.name = "NCamDistortMap";
                    DistortMap.wrapMode = TextureWrapMode.Clamp;
                    DistortMap.Create();  // Call Create() so it's actually uploaded to the GPU
                }
                else
                {
                    yield return null;
                }
            }

            GL.IssuePluginEvent(NCamPlugin.GetNCamRenderEventFunc(), 0);

            yield return null;
        }
    }


    void Update()
    {
        if (Input.GetKeyDown(KeyCode.F3) && Input.GetKey(KeyCode.LeftControl))
            showGUI = !showGUI;


        if (!updateData || !NCamPlugin.NCamIsOpen())
            return;

        if (!NCamPlugin.NCamUpdate())
        {
            return;
        }


        NCamPlugin.NCamFieldIndex(0);

        NCamPlugin.NCamOpticalParameters(ncamOptical.Handle.AddrOfPinnedObject());
        NCamPlugin.NCamEncoderParameters(ncamEncoder.Handle.AddrOfPinnedObject());
      
    }


    void OnGUI()
    {
        if (!showGUI)
            return;

        if (guiWindow == null)
        {
            int w = 390;
            int h = 400;
            guiWindow = new GUIWindow(5, 5, w, h, "NCam LensTable Builder [CTRL + F3]");
        }

        guiWindow.rect = GUILayout.Window(guiWindow.id, guiWindow.rect, BuildWindow, guiWindow.label);
    }


    void BuildWindow(int windowId)
    {
        GUILayout.BeginHorizontal("box");
        {
            ipAddress = GUILayout.TextField(ipAddress);
            GUIAux.Int(port);

            Color c = GUI.color;
            if (NCamPlugin.NCamIsOpen())
            {

                if (!connecting)
                {
                    GUI.color = Color.green;
                    GUILayout.Label("Connected", "box");
                }
                else
                {
                    GUI.color = Color.yellow;
                    GUILayout.Label("Connecting", "box");
                }
            }
            else
            {
                GUI.color = Color.red;
                if (NCamPlugin.ErrorCode() == NCamPlugin.ErrorCodeEnum.None)
                    GUILayout.Label("Disconnected", "box");
                else
                    GUILayout.Label("Error: " + NCamPlugin.ErrorCode().ToString(), "box");
            }
            GUI.color = c;
        }
        GUILayout.EndHorizontal();


        GUILayout.BeginHorizontal("box");
        {
            updateData = GUILayout.Toggle(updateData, "Update");

            if (connecting)
                GUI.enabled = false;

            if (GUILayout.Button("Open"))
            {
                StartCoroutine(OpenNCam());
            }

            if (GUILayout.Button("Close"))
            {
                CloseNCam();
            }

            GUI.enabled = true;
        }
        GUILayout.EndHorizontal();


        GUILayout.BeginVertical("box");
        {
            GUILayout.BeginHorizontal();
            {
                GUILayout.Label("Optical Parameters:     " + this.ncamOptical.TimeCode.ToString(), "box");
            }
            GUILayout.EndHorizontal();


            GUILayout.BeginHorizontal();
            {
                GUILayout.BeginHorizontal("box");
                {
                    GUILayout.Label("Fov(v): ");
                    GUILayout.Label(ncamOptical.FieldOfViewVertical.ToString("0.00"), "box");
                }
                GUILayout.EndHorizontal();
                GUILayout.BeginHorizontal("box");
                {
                    GUILayout.Label("Aspect: ");
                    GUILayout.Label(ncamOptical.ImageAspectRatio.ToString("0.00"), "box");
                }
                GUILayout.EndHorizontal();
                GUILayout.BeginHorizontal("box");
                {
                    GUILayout.Label("Image: ");
                    GUILayout.Label(ncamOptical.ImageResolution.x.ToString(), "box");
                    GUILayout.Label(ncamOptical.ImageResolution.y.ToString(), "box");
                }
                GUILayout.EndHorizontal();
            }
            GUILayout.EndHorizontal();

            GUILayout.BeginHorizontal();
            {
                GUILayout.BeginHorizontal("box");
                {
                    GUILayout.Label("Sensor Size: ");
                    GUILayout.Label(ncamOptical.ImageSensorSize.x.ToString("0.00"), "box");
                    GUILayout.Label(ncamOptical.ImageSensorSize.y.ToString("0.00"), "box");
                }
                GUILayout.EndHorizontal();
                GUILayout.BeginHorizontal("box");
                {
                    GUILayout.Label("Center: ");
                    GUILayout.Label(ncamOptical.ProjectionCenter.x.ToString("0.00"), "box");
                    GUILayout.Label(ncamOptical.ProjectionCenter.y.ToString("0.00"), "box");
                }
                GUILayout.EndHorizontal();
            }
            GUILayout.EndHorizontal();
        }
        GUILayout.EndVertical();

        showEncoderGUI = EncoderGUI(ncamEncoder, "Encoder", showEncoderGUI);

       
        GUI.DragWindow();
    }

   
    bool EncoderGUI(NCamEncoder encoder, string label, bool collapsed)
    {
        GUILayout.BeginVertical("box");
        {
            if (GUILayout.Button((collapsed ? "+ " : "- ") + label, "box"))
                collapsed = !collapsed;

            if (!collapsed)
            {
                GUILayout.BeginHorizontal("box");
                {
                    GUILayout.Label("Zoom: ", GUILayout.Width(100));
                    GUILayout.Label(encoder.GetValue(NCamEncoder.EParameter.ZoomNormalized).ToString("0.00000"), "box");
                    GUILayout.Label(encoder.GetValue(NCamEncoder.EParameter.ZoomMapped).ToString("00000"), "box");
                }
                GUILayout.EndHorizontal();

                GUILayout.BeginHorizontal("box");
                {
                    GUILayout.Label("Focus: ", GUILayout.Width(100));
                    GUILayout.Label(encoder.GetValue(NCamEncoder.EParameter.FocusNormalized).ToString("0.00000"), "box");
                    GUILayout.Label(encoder.GetValue(NCamEncoder.EParameter.FocusMapped).ToString("00000"), "box");
                }
                GUILayout.EndHorizontal();

                GUILayout.BeginHorizontal("box");
                {
                    GUILayout.Label("Iris: ", GUILayout.Width(100));
                    GUILayout.Label(encoder.GetValue(NCamEncoder.EParameter.IrisNormalized).ToString("0.00000"), "box");
                    GUILayout.Label(encoder.GetValue(NCamEncoder.EParameter.IrisMapped).ToString("00000"), "box");
                }
                GUILayout.EndHorizontal();
            }
        }
        GUILayout.EndVertical();

        return collapsed;
    }



    public bool ReadXml(string file_path)
    {
        XmlDocument xmlDoc = new XmlDocument();

        try
        {
            xmlDoc.Load(file_path);

            XmlNodeList docXml = xmlDoc.GetElementsByTagName("NCam");
            XmlNode ncamXml = docXml[0];

            if (ncamXml != null)
            {
                bool isEnabled = false;
                XmlIO.Read(ncamXml.ChildNodes[0], ref isEnabled);
                enabled = isEnabled;
                XmlIO.Read(ncamXml.ChildNodes[1], ref ipAddress);
                XmlIO.Read(ncamXml.ChildNodes[2], ref port);
            }
        }
        catch (System.Exception e)
        {
            Debug.LogError(e.Message);
            return false;
        }
        return true;
    }


}
