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
    private static extern void LensTableProjectionMatrix(float zoom, float focus, System.IntPtr floatArray16_GLProjectionMatrix);
    [DllImport("LensTable")]
    private static extern void LensTableOpticalParameters(float zoom, float focus, System.IntPtr floatArray13_OpticalParameters);
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
    [DllImport("LensTable")]
    private static extern int LensTableDistortionMapWidth();
    [DllImport("LensTable")]
    private static extern int LensTableDistortionMapHeight();
    [DllImport("LensTable")]
    private static extern int LensTableDistortionMapChannelCount();


    private RenderTexture lensMap = null;
    private RenderTexture distortionMap = null;
    private LensProjectionMatrix projectionMatrix = null;
    private LensOptical opticalParameters = null;
    private float fovMin = 0.0f;
    private float fovMax = 0.0f;


    public RenderTexture Map
    {
        get { return lensMap; }
    }

    public RenderTexture DistortionMap
    {
        get { return distortionMap; }
    }

    public Matrix4x4 ProjectionMatrix
    {
        get { return projectionMatrix.Matrix; }
    }

    public LensPlugin()
    {
        LensTableCreate();

        projectionMatrix = new LensProjectionMatrix();
        opticalParameters = new LensOptical();
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
            return (CreateLensMap() && CreateDistortionMap());
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
        lensMap.filterMode = FilterMode.Bilinear; //FilterMode.Point;
        return lensMap.Create();  // Call Create() so it's actually uploaded to the GPU
    }

    private bool CreateDistortionMap()
    {
        // Create RenderTexture for image lens
        distortionMap = new RenderTexture(LensPlugin.LensTableDistortionMapWidth(),
                                    LensPlugin.LensTableDistortionMapHeight(), 8,
                                    RenderTextureFormat.RGFloat);
        distortionMap.name = "DistortionMap";
        distortionMap.wrapMode = TextureWrapMode.Clamp;
        distortionMap.filterMode = FilterMode.Bilinear; //FilterMode.Point;
        return distortionMap.Create();  // Call Create() so it's actually uploaded to the GPU
    }


    public void UpdateLensMap()
    {
        LensTableUpdateLensMap(lensMap.GetNativeTextureID());
    }

    public void UpdateDistortionMap(float zoom, float focus)
    {
        LensTableUpdateDistortionMap(zoom, focus, distortionMap.GetNativeTextureID());
    }

    public void UpdateProjection(float zoom, float focus)
    {
        LensTableProjectionMatrix(zoom, focus, projectionMatrix.Handle.AddrOfPinnedObject());
    }

    public void UpdateOptical(float zoom, float focus)
    {
        LensTableOpticalParameters(zoom, focus, opticalParameters.Handle.AddrOfPinnedObject());
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


public class LensProjectionMatrix
{
    private float[] data = null;
    public GCHandle Handle;

    public LensProjectionMatrix()
    {
        data = new float[16];
        Handle = GCHandle.Alloc(data, GCHandleType.Pinned);
    }

    public LensProjectionMatrix(float[] matrix_array_16)
    {
        data = matrix_array_16;
        Handle = GCHandle.Alloc(data, GCHandleType.Pinned);
    }

    ~LensProjectionMatrix()
    {
        Handle.Free();
    }

    public Matrix4x4 Matrix
    {
        get
        {
            Matrix4x4 mat = Matrix4x4.identity;
            for (int i = 0; i < 16; ++i)
                mat[i] = data[i];
            return mat;
        }
    }

    public float[] DataArray
    {
        get { return data; }
        set { data = value; }
    }
}



public class LensOptical
{
    private float[] data = null;
    public GCHandle Handle;

    public LensOptical()
    {
        this.data = new float[13];
        this.Handle = GCHandle.Alloc(data, GCHandleType.Pinned);
    }

    public LensOptical(float[] optical_data_array_13)
    {
        this.data = optical_data_array_13;
        this.Handle = GCHandle.Alloc(data, GCHandleType.Pinned);
    }

    ~LensOptical()
    {
        this.Handle.Free();
    }

    public float[] DataArray
    {
        get { return this.data; }
        set { this.data = value; }
    }

    public float FieldOfViewVertical
    {
        get { return this.data[1]; }
    }

    public Vector2 ProjectionCenter
    {
        get { return new Vector2(this.data[2], this.data[3]); }
    }

    public Vector2 ImageSensorSize
    {
        get { return new Vector2(this.data[4], this.data[5]); }
    }

    public Vector2 ImageResolution
    {
        get { return new Vector2(this.data[6], this.data[7]); }
    }

    public float ImageAspectRatio
    {
        get { return this.data[8]; }
    }
}

