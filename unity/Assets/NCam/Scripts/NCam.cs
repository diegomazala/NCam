
using UnityEngine;
using System.Collections;
using System.Runtime.InteropServices;
using System.Xml;



public class NCam : MonoBehaviour
{
    public string ipAddress = "127.0.0.1";
    public int port = 38860; //3003;

    public bool useGLMatrix = false;
    public float scale = 100.0f;
    

    public Camera[] targetCamera = { null, null };

    private Vector3 eulerAngles = Vector3.zero;
    private Vector3 position = Vector3.zero;

    private Queue dataBuffer = new Queue();
    private NCamData[] ncamData = { null, null };

    public NCamEncoder[] ncamEncoder = { null, null };

    public NCamOptical[] ncamOptical = { null, null };
    public NCamTracking[] ncamTracking = { null, null };

    public NCamMatrix[] ncamProjection = { null, null };
    public NCamMatrix[] ncamModelview = { null, null };

    [HideInInspector]
    public RenderTexture distortionMap = null;
    [HideInInspector]
    public Vector2 distortionMapSize;

    [HideInInspector]
    public int FieldIndex = 0;


    public bool reconnecting = false;
    public bool autoConnection = false;
    private uint lastOpticalTimeCode = 0;


    //#region GUI Variables
    //private GUIWindow guiWindow = null;
    //public bool showGUI = false;
    //private bool showMatrices = false;
    //private bool showParentTransform = false;
    //#endregion


    public enum Status
    {
        Disconnected,
        Connected,
        Connecting
    }

    public Status GetStatus()
    {
        if (reconnecting)
            return Status.Connecting;
        else if (NCamPlugin.NCamIsOpen())
            return Status.Connected;
        else
            return Status.Disconnected;
    }


    public void Connect()
    {
        if (!NCamPlugin.NCamIsOpen() && !reconnecting)
            StartCoroutine(StartNCam());
    }

    public void Disconnect()
    {
        StopNCam();
    }

    public int FrameSync
    {
        get { return (int)(ncamData[1].OpticalTimeCode.Time - ncamData[0].OpticalTimeCode.Time) - 1; }
    }

    public int FieldSync
    {
        get { return (int)(ncamData[1].OpticalTimeCode.Time - lastOpticalTimeCode) - 2; }
    }

    public int DroppedFrames
    {
        get { return NCamPlugin.NCamFrameLostCount(); }
    }

    public int DroppedFields
    {
        get { return NCamPlugin.NCamFieldLostCount(); }
    }

    public int FrameDelay
    {
        get;
        set;
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

    public NCamEncoder Encoder
    {
        get { return ncamEncoder[FieldIndex]; }
    }

    public NCamOptical Optical
    {
        get { return ncamOptical[FieldIndex]; }
    }

    public NCamTracking Tracking
    {
        get { return ncamTracking[FieldIndex]; }
    }

    public NCamMatrix Projection
    {
        get { return ncamProjection[FieldIndex]; }
    }

    public NCamMatrix Modelview
    {
        get { return ncamModelview[FieldIndex]; }
    }

    public Vector3 Position
    {
        get { return position; }
    }

    public Vector3 Rotation
    {
        get { return eulerAngles; }
    }

    public float ProjectionMatrix2Fovy(float mat_11)
    {
        float m = Mathf.Pow(mat_11, -1);
        return Mathf.Atan(m) * Mathf.Rad2Deg * 2.0f;
    }

    public bool Distortion
    {
        set
        {
            for (int i = 0; i < 2; ++i)
            {
                NCamDistortion dist = targetCamera[i].GetComponent<NCamDistortion>() as NCamDistortion;
                if (dist != null)
                    dist.enabled = value;
            }
        }

        get
        {
            NCamDistortion dist = targetCamera[1].GetComponent<NCamDistortion>() as NCamDistortion;
            if (dist != null)
                return dist.enabled;
            else
                return false;
        }
    }

    void Awake()
    {
        ncamEncoder = new NCamEncoder[2];
        ncamOptical = new NCamOptical[2];
        ncamTracking = new NCamTracking[2];
        ncamProjection = new NCamMatrix[2];
        ncamModelview = new NCamMatrix[2];
        ncamData = new NCamData[2];
        distortionMapSize = Vector2.one;

        for (int i = 0; i < 2; ++i)
        {
            ncamEncoder[i] = new NCamEncoder();
            ncamOptical[i] = new NCamOptical();
            ncamTracking[i] = new NCamTracking();
            ncamProjection[i] = new NCamMatrix();
            ncamModelview[i] = new NCamMatrix();

            ncamData[i] = new NCamData(new NCamTimeCode(), new NCamTimeCode(), 45.0f, 1.78f,
                                        new Vector2(1920.0f, 1080.0f), new Vector2(9.59f, 5.39f),
                                        new Vector2(0.0f, 0.0f),
                                        distortionMapSize,
                                        Matrix4x4.Perspective(45.0f, 1.78f, 0.1f, 1024.0f),
                                        Matrix4x4.identity);
            if (enabled)
            {
                NCamDistortion dist = targetCamera[i].GetComponent<NCamDistortion>() as NCamDistortion;
                if (dist != null)
                {
                    dist.ncam = this;
                    dist.enabled = false;
                }
            }
        }
    }





    void OnEnable()
    {
        NCamPlugin.NCamCreate();

        if (targetCamera.Length != 2 || targetCamera[0] == null || targetCamera[1] == null)
        {
            Debug.LogError("Missing reference to camera");
            enabled = false;
        }

#if !UNITY_EDITOR
        // If the xml configuration file does not exists, create it
        if (!ReadXml(xmlConfigFolder + @"NCam.xml"))
        {
            System.IO.DirectoryInfo sensorDir = new System.IO.DirectoryInfo(xmlConfigFolder);
            if (!sensorDir.Exists)
            {
                sensorDir.Create();
            }
            WriteXml(xmlConfigFolder + @"NCam.xml");
        }
#endif

        Connect();
    }


    void OnDisable()
    {
        Disconnect();

#if !UNITY_EDITOR
        //WriteXml(xmlConfigFolder + @"NCam.xml");
#endif

        NCamPlugin.NCamDestroy();
    }


    private IEnumerator CreateDistortMap()
    {
        int w = NCamPlugin.NCamDistortMapWidth();
        int h = NCamPlugin.NCamDistortMapHeight();

        distortionMapSize = new Vector2(w, h);

        if (w > 1 && h > 1) // otherwise the reading has not been initialized yet
        {
            distortionMap = new RenderTexture(w, h, 8, RenderTextureFormat.RGFloat);
            distortionMap.name = "NCamDistortMap";
            distortionMap.anisoLevel = 0;
            distortionMap.wrapMode = TextureWrapMode.Clamp;
            distortionMap.Create();  // Call Create() so it's actually uploaded to the GPU

            NCamPlugin.NCamSetDistortMapPtr(distortionMap.GetNativeTexturePtr());
        }

        yield return new WaitForEndOfFrame();
    }


    private IEnumerator StartNCam()
    {
        reconnecting = true;
        yield return new WaitForEndOfFrame();
        StopNCam();

        NCamPlugin.NCamResetError();
        NCamPlugin.NCamSetPacketType(true, true, true);

        yield return new WaitForEndOfFrame();

        NCamPlugin.NCamSetIpAddress(ipAddress, port);
        GL.IssuePluginEvent(NCamPlugin.GetNCamRenderEventFunc(), (int)NCamRenderEvent.Initialize);

        yield return new WaitForEndOfFrame();
        reconnecting = false;

        while (true)
        {
            // Wait until all frame rendering is done
            yield return new WaitForEndOfFrame();

            if (!NCamPlugin.NCamIsOpen())
                yield return null;

            if (distortionMap == null)
                yield return CreateDistortMap();

            GL.IssuePluginEvent(NCamPlugin.GetNCamRenderEventFunc(), (int)NCamRenderEvent.Update);

            // Check if we got a valid packet
            if (NCamPlugin.NCamOpticalTimeCode(Optical.Handle.AddrOfPinnedObject()) > 0)
            {
                UpdateCameras();
                GL.IssuePluginEvent(NCamPlugin.GetNCamRenderEventFunc(), (int)NCamRenderEvent.UpdateDistortion);
            }
        }
    }



    private void StopNCam()
    {
        reconnecting = false;
        StopCoroutine(StartNCam());
        GL.IssuePluginEvent(NCamPlugin.GetNCamRenderEventFunc(), (int)NCamRenderEvent.Uninitialize);
    }





    void UpdateCameras()
    {
        lastOpticalTimeCode = ncamData[1].OpticalTimeCode.Time;

        for (int i = 0; i < 2; ++i)
        {
            FieldIndex = i;
            NCamPlugin.NCamFieldIndex(FieldIndex);
            NCamPlugin.NCamGetData(Optical.TimeCode.Handle.AddrOfPinnedObject(),
                                Tracking.TimeCode.Handle.AddrOfPinnedObject(),
                                Encoder.Handle.AddrOfPinnedObject(),
                                Optical.Handle.AddrOfPinnedObject(),
                                Tracking.Handle.AddrOfPinnedObject(),
                                Projection.Handle.AddrOfPinnedObject(),
                                Modelview.Handle.AddrOfPinnedObject(),
                                targetCamera[FieldIndex].nearClipPlane, targetCamera[FieldIndex].farClipPlane);

            dataBuffer.Enqueue(new NCamData(Tracking.TimeCode, Optical.TimeCode,
                                        Optical.FieldOfViewVertical, Optical.ImageAspectRatio,
                                        Optical.ImageResolution, Optical.ImageSensorSize,
                                        Optical.ProjectionCenter, distortionMapSize,
                                        Projection.Matrix, Modelview.Matrix));

            if (dataBuffer.Count > FrameDelay)
                ncamData[FieldIndex] = dataBuffer.Dequeue() as NCamData;
        }


        if (!useGLMatrix)
        {
            for (int i = 0; i < 2; ++i)
            {
                // Reseting matrices in order to have gizmos update into Unity
                targetCamera[i].ResetProjectionMatrix();
                targetCamera[i].ResetWorldToCameraMatrix();

                // Projection Setup
                targetCamera[i].fieldOfView = ProjectionMatrix2Fovy(-ncamData[i].GLProjection.m11);
                // There is a number precision difference between ImageAspectRatio and the ration between m11 and m00
                targetCamera[i].aspect = Mathf.Abs(ncamData[i].GLProjection.m11 / ncamData[i].GLProjection.m00); // ncamData[i].AspectRatio; 

                ApplyCcdShift(targetCamera[i], -ncamData[i].GLProjection.m02, -ncamData[i].GLProjection.m12);

                // ModelView Setup
                Matrix4x4 mdv = ncamData[i].GLModelView.inverse;
                position = mdv.GetColumn(3);
                eulerAngles = QuaternionFromMatrix(mdv).eulerAngles;
                targetCamera[i].transform.localPosition = new Vector3(position.x * scale, position.y * scale, -position.z * scale);
                targetCamera[i].transform.localRotation = Quaternion.Euler(eulerAngles.x, 180.0f - eulerAngles.y, 180.0f - eulerAngles.z);

            }
        }
        else
        {
            for (int i = 0; i < 2; ++i)
            {
                targetCamera[i].projectionMatrix = ncamData[i].GLProjection;
                targetCamera[i].worldToCameraMatrix = ncamData[i].GLModelView;

                // invalid values
                position = Vector3.zero;
                eulerAngles = Vector3.zero;
            }
        }

        while (dataBuffer.Count > FrameDelay)
        {
            dataBuffer.Dequeue();
        }

    }

    void Update()
    {
        //if (Input.GetKeyDown(KeyCode.F3) && Input.GetKey(KeyCode.LeftControl))
        //    showGUI = !showGUI;


        if (!NCamPlugin.NCamIsOpen() && !reconnecting && autoConnection)
        {
            StartCoroutine(StartNCam());
        }

    }

    
    public void ApplyCcdShift(Camera cam, float ccd_shift_x, float ccd_shift_y)
    {
        Matrix4x4 p = cam.projectionMatrix;

        p[0, 2] = ccd_shift_x;
        p[1, 2] = ccd_shift_y;

        cam.projectionMatrix = p;
    }

    public static Quaternion QuaternionFromMatrix(Matrix4x4 m)
    {
        return Quaternion.LookRotation(m.GetColumn(2), m.GetColumn(1));
    }


    public static Vector3 EulerAnglesFromMatrix(Matrix4x4 m)
    {
        return Quaternion.LookRotation(m.GetColumn(2), m.GetColumn(1)).eulerAngles;
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
                XmlIO.Read(ncamXml.ChildNodes[3], ref autoConnection);
                int frameDelay = 0;
                XmlIO.Read(ncamXml.ChildNodes[4], ref frameDelay);
                FrameDelay = FrameDelay;
                XmlIO.Read(ncamXml.ChildNodes[5], ref scale);
                XmlIO.Read(ncamXml.ChildNodes[6], ref useGLMatrix);
                bool useDistortion = Distortion;
                XmlIO.Read(ncamXml.ChildNodes[7], ref useDistortion);
                Distortion = useDistortion;

				if (enabled)
				{	
					XmlNode localPosNode = ncamXml.ChildNodes[8];
					{
						Vector3 localPos = Vector3.zero;
						XmlIO.Read(localPosNode.ChildNodes[0], ref localPos.x);
						XmlIO.Read(localPosNode.ChildNodes[1], ref localPos.y);
						XmlIO.Read(localPosNode.ChildNodes[2], ref localPos.z);
						this.transform.localPosition = localPos;
					}

					XmlNode localRotNode = ncamXml.ChildNodes[9];
					{
						Vector3 localRot = Vector3.zero;
						XmlIO.Read(localRotNode.ChildNodes[0], ref localRot.x);
						XmlIO.Read(localRotNode.ChildNodes[1], ref localRot.y);
						XmlIO.Read(localRotNode.ChildNodes[2], ref localRot.z);
						this.transform.localRotation = Quaternion.Euler(localRot);
					}
				}
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

        XmlElement ncamXml = xmlDoc.CreateElement("NCam");

        XmlIO.Write(ncamXml, enabled, "Enabled");
        XmlIO.Write(ncamXml, ipAddress, "IpAddress");
        XmlIO.Write(ncamXml, port, "Port");
        XmlIO.Write(ncamXml, autoConnection, "AutoConnection");
        XmlIO.Write(ncamXml, FrameDelay, "DelayInFrames");
        XmlIO.Write(ncamXml, scale, "ScaleUnits");
        XmlIO.Write(ncamXml, useGLMatrix, "UseGLMatrix");
        XmlIO.Write(ncamXml, Distortion, "Distortion");

		if (enabled)
		{
			XmlElement xmlLocalPosition = xmlDoc.CreateElement("LocalPosition");
			XmlIO.WriteFields(xmlLocalPosition, this.transform.localPosition);
			ncamXml.AppendChild(xmlLocalPosition);

			XmlElement xmlLocalRotation = xmlDoc.CreateElement("LocalRotation");
			XmlIO.WriteFields(xmlLocalRotation, this.transform.localRotation.eulerAngles);
			ncamXml.AppendChild(xmlLocalRotation);
		}
		
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




#if false
    void OnGUI()
    {
        if (!showGUI)
            return;

        if (guiWindow == null)
        {
            int w = 390;
            int h = 680;
            guiWindow = new GUIWindow(Screen.width - w - 10, Screen.height - h - 10, w, h, "NCam [CTRL + F3]");
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

                if (!reconnecting)
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
            if (autoConnection)
                GUI.enabled = false;

            if (GUILayout.Button("Open"))
            {
                StartCoroutine(StartNCam());
            }

            if (GUILayout.Button("Close"))
            {
                StopNCam();
            }

            GUI.enabled = true;
            GUILayout.Space(15);
            autoConnection = GUILayout.Toggle(autoConnection, "Auto Connection", GUILayout.Width(130));
        }
        GUILayout.EndHorizontal();


        GUILayout.BeginHorizontal("box");
        {
            useGLMatrix = GUILayout.Toggle(useGLMatrix, "Use OpenGL Matrix");

            bool dist = Distortion;
            dist = GUILayout.Toggle(dist, "Distortion");
            if (dist != Distortion)
                Distortion = dist;
        }
        GUILayout.EndHorizontal();

        GUILayout.BeginHorizontal("box");
        {
            if (GUILayout.Button("Buffer:" + dataBuffer.Count.ToString()))
                dataBuffer.Clear();
            GUILayout.Label("Frame Delay:");
            FrameDelay = GUIAux.Int(FrameDelay, "box");
            GUILayout.Label("Scale Units:");
            scale = GUIAux.Float(scale, "box");

            GUIAux.Int((int)(ncamData[1].OpticalTimeCode.Time - ncamData[0].OpticalTimeCode.Time));
            GUIAux.Int((int)(ncamData[1].OpticalTimeCode.Time - lastOpticalTimeCode));
        }
        GUILayout.EndHorizontal();

        GUILayout.BeginVertical("box");
        {
            GUILayout.BeginHorizontal();
            {
                GUILayout.Label("Optical Parameters:     " + ncamData[FieldIndex].OpticalTimeCode.ToString(), "box");
            }
            GUILayout.EndHorizontal();

            if (useGLMatrix)
            {
                GUILayout.BeginVertical("box");
                {
                    GUILayout.Label("OpenGL Projection Matrix", "box");
                    GUILayout.Label(ncamData[FieldIndex].GLProjection.ToString(), "box");
                }
                GUILayout.EndVertical();
            }
            else
            {
                GUILayout.BeginHorizontal();
                {
                    GUILayout.BeginHorizontal("box");
                    {
                        GUILayout.Label("Fov(v): ");
                        GUILayout.Label(ncamData[FieldIndex].Fov.ToString("0.0"), "box");
                    }
                    GUILayout.EndHorizontal();
                    GUILayout.BeginHorizontal("box");
                    {
                        GUILayout.Label("Aspect: ");
                        GUILayout.Label(ncamData[FieldIndex].AspectRatio.ToString("0.00"), "box");
                    }
                    GUILayout.EndHorizontal();
                    GUILayout.BeginHorizontal("box");
                    {
                        GUILayout.Label("Image: ");
                        GUILayout.Label(ncamData[FieldIndex].ImageResolution.x.ToString(), "box");
                        GUILayout.Label(ncamData[FieldIndex].ImageResolution.y.ToString(), "box");
                    }
                    GUILayout.EndHorizontal();
                }
                GUILayout.EndHorizontal();

                GUILayout.BeginHorizontal();
                {
                    GUILayout.BeginHorizontal("box");
                    {
                        GUILayout.Label("Sensor Size: ");
                        GUILayout.Label(ncamData[FieldIndex].ImageSensorSize.x.ToString("0.00"), "box");
                        GUILayout.Label(ncamData[FieldIndex].ImageSensorSize.y.ToString("0.00"), "box");
                    }
                    GUILayout.EndHorizontal();
                    GUILayout.BeginHorizontal("box");
                    {
                        GUILayout.Label("Center: ");
                        GUILayout.Label(ncamData[FieldIndex].ProjectionCenter.x.ToString("0.00"), "box");
                        GUILayout.Label(ncamData[FieldIndex].ProjectionCenter.y.ToString("0.00"), "box");
                    }
                    GUILayout.EndHorizontal();
                }
                GUILayout.EndHorizontal();
            }
        }
        GUILayout.EndVertical();

        GUILayout.BeginVertical("box");
        {
            GUILayout.BeginHorizontal();
            {
                GUILayout.Label("Tracking Parameters:     " + ncamData[FieldIndex].TrackingTimeCode.ToString(), "box");
            }
            GUILayout.EndHorizontal();

            if (useGLMatrix)
            {
                GUILayout.BeginVertical("box");
                {
                    GUILayout.Label("OpenGL ModelView Matrix", "box");
                    GUILayout.Label(ncamData[FieldIndex].GLModelView.ToString(), "box");
                }
                GUILayout.EndVertical();
            }
            else
            {
                GUILayout.BeginVertical("box");
                {
                    GUILayout.Label("NCam Values", "box");
                    GUILayout.BeginHorizontal("box");
                    {
                        GUILayout.Label("Position: ");
                        GUILayout.Label(position.ToString(), "box");
                    }
                    GUILayout.EndHorizontal();
                    GUILayout.BeginHorizontal("box");
                    {
                        GUILayout.Label("Rotation: ");
                        GUILayout.Label(eulerAngles.ToString(), "box");
                    }
                    GUILayout.EndHorizontal();
                }
                GUILayout.EndVertical();

                GUILayout.BeginVertical("box");
                {
                    GUILayout.Label("Computed Values", "box");
                    GUILayout.BeginHorizontal("box");
                    {
                        GUILayout.Label("Position: ");
                        GUILayout.Label(targetCamera[0].transform.localPosition.ToString(), "box");
                    }
                    GUILayout.EndHorizontal();
                    GUILayout.BeginHorizontal("box");
                    {
                        GUILayout.Label("Rotation: ");
                        GUILayout.Label(targetCamera[0].transform.localEulerAngles.ToString(), "box");
                    }
                    GUILayout.EndHorizontal();
                }
                GUILayout.EndVertical();
            }


            showParentTransform = TransformGUI(this.transform, "Parent Transform", showParentTransform);

            if (GUILayout.Button((showMatrices ? "-" : "+") + " Matrices", "box"))
                showMatrices = !showMatrices;

            if (showMatrices)
            {
                GUILayout.Label(targetCamera[0].projectionMatrix.ToString(), "box");
                GUILayout.Label(targetCamera[0].worldToCameraMatrix.ToString(), "box");
            }
        }
        GUILayout.EndVertical();

        GUI.DragWindow();
    }

    bool TransformGUI(Transform t, string label, bool collapsed)
    {
        GUILayout.BeginVertical("box");
        {
            if (GUILayout.Button((collapsed ? "+ " : "- ") + label, "box"))
                collapsed = !collapsed;

            if (!collapsed)
            {
                GUILayout.BeginHorizontal("box");
                {
                    GUILayout.Label("Position: ");
                    t.localPosition = GUIAux.Vector3(t.localPosition, "box");
                }
                GUILayout.EndHorizontal();
                GUILayout.BeginHorizontal("box");
                {
                    GUILayout.Label("Rotation: ");
                    t.localEulerAngles = GUIAux.Vector3(t.localEulerAngles, "box");
                }
                GUILayout.EndHorizontal();
            }
        }
        GUILayout.EndVertical();

        return collapsed;
    }
#endif
}
