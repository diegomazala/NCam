using UnityEngine;
using System.Collections;
using System;
using System.Xml;


public class LensTable : MonoBehaviour
{
    public string fileName = "emulator.lens";
    public Camera[] targetCamera = { null, null };
    private LensDistortionUVMap[] lensDistortion = { null, null };

    private LensPlugin lens;

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


        lens = new LensPlugin();

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



    void Update()
    {
        // just for DEBUG
        for (int i = 0; i < targetCamera.Length; ++i)
        {
            Camera cam = targetCamera[i];
            LensDistortionUVMap ld = lensDistortion[i];

            if (ld != null)
            {
                ld.Zoom = zoom;
                ld.Focus = focus;
                fovMapped =
                cam.fieldOfView = lens.FieldOfView(ld.Fov);
            }
        }
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
}
