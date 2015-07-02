using UnityEngine;
using System.Collections;
using System;
using System.Xml;


public class LensTable : MonoBehaviour
{
    public string fileName = "emulator.lens";
    public Camera[] targetCamera = { null, null };
    private LensDistortionUVMap[] lensDistortion = { null, null };

    private LensPlugin lens = new LensPlugin();

    public float zoom = 0;
    public float focus = 0;
    public float fovMapped = 0;

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
        if (!ReadXml(xmlConfigFolder + @"Lens.xml"))
        {
            System.IO.DirectoryInfo lensDir = new System.IO.DirectoryInfo(xmlConfigFolder);
            if (!lensDir.Exists)
            {
                lensDir.Create();
            }
            WriteXml(xmlConfigFolder + @"Lens.xml");
        }
#endif

        string filePath = xmlConfigFolder + fileName;

        if (lens.ReadFile(filePath))
        {
            if (SystemInfo.graphicsDeviceVersion.Contains("OpenGL"))
            {
                StartCoroutine(UpdateLensMap());
                StartCoroutine(UpdateDistortionMap());
            } 
        }
        else
        {
            Debug.LogError("Could not open lens file: " + filePath);
        }

        
    }


    void OnDisable()
    {
        StopCoroutine( UpdateDistortionMap() );
        StopCoroutine( UpdateLensMap() );
    }


    private IEnumerator UpdateLensMap()
    {
        yield return new WaitForEndOfFrame();

        // BUG: Why do I have to call twice?
        lens.UpdateLensMap();
        lens.UpdateLensMap();

        for (int i = 0; i < targetCamera.Length; ++i)
        {
            if (lensDistortion[i] == null)
                lensDistortion[i] = targetCamera[i].GetComponent<LensDistortionUVMap>();
            if (lensDistortion[i] == null)
                lensDistortion[i] = targetCamera[i].gameObject.AddComponent<LensDistortionUVMap>();

            lensDistortion[i].lensMap = lens.Map;
            lensDistortion[i].distortionMap = lens.DistortionMap;
        }

        
        
        yield return null;
    }

    private IEnumerator UpdateDistortionMap()
    {
        yield return new WaitForEndOfFrame();
        yield return new WaitForEndOfFrame();

        while (enabled)
        {
            // Wait until all frame rendering is done
            yield return new WaitForEndOfFrame();

            for (int i = 0; i < targetCamera.Length; ++i)
                lens.UpdateDistortionMap(zoom, focus);
        }

        yield return null;
    }



    public void UpdateCameraLens(float _zoom, float _focus)
    {
        for (int i = 0; i < targetCamera.Length; ++i)
        {
            Camera cam = targetCamera[i];
            LensDistortionUVMap ld = lensDistortion[i];


            //cam.ResetProjectionMatrix();
            //// Projection Setup
            //cam.fieldOfView = ProjectionMatrix2Fovy(-ncamData[i].GLProjection.m11);
            //// There is a number precision difference between ImageAspectRatio and the ration between m11 and m00
            //targetCamera[i].aspect = Mathf.Abs(.m11 / ncamData[i].GLProjection.m00); // ncamData[i].AspectRatio; 
            //ApplyCcdShift(targetCamera[i], -ncamData[i].GLProjection.m02, -ncamData[i].GLProjection.m12);

            if (ld != null)
            {
                ld.Zoom = _zoom;
                ld.Focus = _focus;
                fovMapped =
                cam.fieldOfView = lens.FieldOfView(ld.Fov);
            }
        }
    }

    void Update()
    {
        UpdateCameraLens(zoom, focus);
    }



    public bool ReadXml(string file_path)
    {
        XmlDocument xmlDoc = new XmlDocument();

        try
        {
            xmlDoc.Load(file_path);

            XmlNodeList docXml = xmlDoc.GetElementsByTagName("Lens");
            XmlNode ncamXml = docXml[0];

            if (ncamXml != null)
            {
                bool isEnabled = false;
                XmlIO.Read(ncamXml.ChildNodes[0], ref isEnabled);
                enabled = isEnabled;
                XmlIO.Read(ncamXml.ChildNodes[1], ref fileName);
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

        XmlElement ncamXml = xmlDoc.CreateElement("Lens");

        XmlIO.Write(ncamXml, enabled, "Enabled");
        XmlIO.Write(ncamXml, fileName, "FileName");

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

    static public float ProjectionMatrix2Fovy(float mat_11)
    {
        float m = Mathf.Pow(mat_11, -1);
        return Mathf.Atan(m) * Mathf.Rad2Deg * 2.0f;
    }

    static public void ApplyCcdShift(Camera cam, float ccd_shift_x, float ccd_shift_y)
    {
        Matrix4x4 p = cam.projectionMatrix;

        p[0, 2] = ccd_shift_x;
        p[1, 2] = ccd_shift_y;

        cam.projectionMatrix = p;
    }
}
