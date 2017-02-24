
using UnityEngine;
using System.Collections;
using System.Runtime.InteropServices;
using System.Xml;



public class NCam : MonoBehaviour
{
    public NCamUI Ui = null;
    public NCamConfig Config = new NCamConfig();

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
    private uint lastOpticalTimeCode = 0;
    

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

    public double FrameRate
    {
        get { return NCamPlugin.NCamFrameRate(); }
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
                    dist.ncam = this;
            }
        }

        if (Ui == null)
        {
            Ui = FindObjectOfType<NCamUI>();
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
        if (!Config.Load())
            Config.Save();
#endif

        if (Ui)
            Ui.gameObject.SetActive(enabled);

        if (enabled)
            Connect();
    }


    void OnDisable()
    {
        Disconnect();

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

        NCamPlugin.NCamSetIpAddress(Config.Ip, Config.Port);
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


        if (!Config.UseGLMatrix)
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
                targetCamera[i].transform.localPosition = new Vector3(position.x * Config.Scale, position.y * Config.Scale, -position.z * Config.Scale);
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


        if (!NCamPlugin.NCamIsOpen() && !reconnecting && Config.AutoConnection)
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

}



