using UnityEngine;
using System.Collections;
using System;


public class LensTable : MonoBehaviour
{
    public Camera[] targetCamera = { null, null };
    private LensDistortion[] lensDistortion = { null, null };

    private LensPlugin lens;

    public float zoom = 0;
    public float focus = 0;
    public float fovMapped = 0;

    void OnEnable()
    {
        lens = new LensPlugin();

        string filePath = @"G:\VgpCode\NCam\data\emulator.lens";

        if (lens.ReadFile(filePath))
        {
            if (SystemInfo.graphicsDeviceVersion.Contains("OpenGL"))
                StartCoroutine(UpdateLensMap());
        }
        else
        {
            Debug.LogError("Could not open lens file: " + filePath);
        }

        
    }


    void OnDisable()
    {
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
                lensDistortion[i] = targetCamera[i].GetComponent<LensDistortion>();
            if (lensDistortion[i] == null)
                lensDistortion[i] = targetCamera[i].gameObject.AddComponent<LensDistortion>();

            lensDistortion[i].lensMap = lens.Map;
        }

        
        
        yield return null;
    }



    void Update()
    {
        // just for DEBUG
        for (int i = 0; i < targetCamera.Length; ++i )
        {
            Camera cam = targetCamera[i];
            LensDistortion ld = lensDistortion[i];

            if (ld != null)
            {
                ld.Zoom = zoom;
                ld.Focus = focus;
                fovMapped = 
                cam.fieldOfView = lens.FieldOfView(ld.Fov);
            }
        }
    }

}
