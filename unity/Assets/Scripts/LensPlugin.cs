using UnityEngine;
using System.Collections;
using System.Runtime.InteropServices;


//[System.Serializable]
//public class LensSample
//{
//    public float zoom;
//    public float focus;
//    public float iris;
//    public float fov;
//    public RenderTexture distortionMap;
//}

    

public class LensPlugin 
{
    [DllImport("LensTable")]
    private static extern void LensTableCreate();
    [DllImport("LensTable")]
    private static extern void LensTableDestroy();
    [DllImport("LensTable")]
    private static extern bool LensTableReadFile(string filepath);
    [DllImport("LensTable")]
    private static extern int LensTableRowCount();
    [DllImport("LensTable")]
    private static extern int LensTableColumnCount();
    [DllImport("LensTable")]
    private static extern float LensTableFovMin();
    [DllImport("LensTable")]
    private static extern float LensTableFovMax();
    [DllImport("LensTable")]
    private static extern bool LensTableUpdateLensMap(int texId);
    [DllImport("LensTable")]
    private static extern bool LensTableUpdateSample(float zoom, float focus, float iris, int texId);
    [DllImport("LensTable")]
    private static extern float LensTableZoom();
    [DllImport("LensTable")]
    private static extern float LensTableFocus();
    [DllImport("LensTable")]
    private static extern float LensTableFov();
    [DllImport("LensTable")]
    private static extern float LensTableIris();
    [DllImport("LensTable")]
    private static extern bool LensTableUpdateDistortionMap(float zoom, float focus, int dist_tex_id);

    private RenderTexture lensMap = null;
    private float fovMin = 0.0f;
    private float fovMax = 0.0f;

    public RenderTexture Map
    {
        get { return lensMap; }
    }

    public LensPlugin()
    {
        LensTableCreate();
    }

    ~LensPlugin()
    {
        LensTableDestroy();
    }


    public bool ReadFile(string filepath)
    {
        if (LensTableReadFile(filepath))
        {
            fovMin = LensTableFovMin();
            fovMax = LensTableFovMax();
            return CreateLensMap();
        }
        else
        {
            return false;
        }
    }


    private bool CreateLensMap()
    {
        // Create RenderTexture for image lens
        lensMap = new RenderTexture(LensPlugin.LensTableColumnCount(),
                                    LensPlugin.LensTableRowCount(), 8,
                                    RenderTextureFormat.ARGB32);
        lensMap.name = "LensMap";
        lensMap.wrapMode = TextureWrapMode.Clamp;
        lensMap.filterMode = FilterMode.Point;
        return lensMap.Create();  // Call Create() so it's actually uploaded to the GPU
    }


    public void UpdateLensMap()
    {
        LensTableUpdateLensMap(lensMap.GetNativeTextureID());
    }


    public float FieldOfView(float fov_normalized)
    {
        return Lerp(fov_normalized, 0.0f, 1.0f, fovMin, fovMax);
    }


    static float Lerp(float x, float x0, float x1, float y0, float y1)
    {
        if ((x1 - x0) == 0)
            return (y0 + y1) / 2;
        return y0 + (x - x0) * (y1 - y0) / (x1 - x0);
    }

}


