using UnityEngine;
using System.Collections;
using System.Runtime.InteropServices;


public class LensPlugin 
{
    [DllImport("LensTable")]
    private static extern void LensTableCreate();
    [DllImport("LensTable")]
    private static extern void LensTableDestroy();
    [DllImport("LensTable")]
    private static extern bool LensTableReadFile(string filepath, bool compute_fov_from_matrix = true);
    [DllImport("LensTable")]
    private static extern int LensTableRowCount();
    [DllImport("LensTable")]
    private static extern int LensTableColumnCount();
    [DllImport("LensTable")]
    private static extern float LensTableFovMin();
    [DllImport("LensTable")]
    private static extern float LensTableFovMax();
    [DllImport("LensTable")]
    private static extern float LensTableUpdate(float zoom, float focus);
    [DllImport("LensTable")]
    private static extern float LensTableFov();
    [DllImport("LensTable")]
    private static extern void LensTableProjection(System.IntPtr floatArray16_GLProjectionMatrix);
    [DllImport("LensTable")]
    private static extern bool LensTableUpdateDistortionMap(float zoom, float focus, int dist_tex_id);
    [DllImport("LensTable")]
    private static extern int LensTableDistortionMapWidth();
    [DllImport("LensTable")]
    private static extern int LensTableDistortionMapHeight();
    [DllImport("LensTable")]
    private static extern int LensTableDistortionMapChannelCount();
    [DllImport("LensTable")]
    private static extern int LensTableDistortionData(System.IntPtr floatArray);


    private RenderTexture distortionMap = null;
    private LensProjection projectionMatrix = null;
    private float fovMin = 0.0f;
    private float fovMax = 0.0f;


    public RenderTexture DistortionMap
    {
        get { return distortionMap; }
    }

    public LensProjection Projection
    {
        get { return projectionMatrix; }
    }

    public LensPlugin()
    {
        LensTableCreate();

        projectionMatrix = new LensProjection();
    }

    ~LensPlugin()
    {
        LensTableDestroy();
    }


    public bool ReadFile(string filepath, bool compute_fov_from_matrix = true)
    {
        if (LensTableReadFile(filepath, compute_fov_from_matrix))
        {
            fovMin = LensTableFovMin();
            fovMax = LensTableFovMax();
            return CreateDistortionMap();
        }
        else
        {
            return false;
        }
    }


    private bool CreateDistortionMap()
    {
        // Create RenderTexture for distortion
        distortionMap = new RenderTexture(LensPlugin.LensTableDistortionMapWidth(),
                                    LensPlugin.LensTableDistortionMapHeight(), 8,
                                    RenderTextureFormat.RGFloat);
        distortionMap.name = "DistortionMap";
        distortionMap.wrapMode = TextureWrapMode.Clamp;
        distortionMap.filterMode = FilterMode.Bilinear; //FilterMode.Point;
        return distortionMap.Create();  // Call Create() so it's actually uploaded to the GPU
    }



    public void Update(float zoom, float focus)
    {
        LensTableUpdate(zoom, focus);
    }

    public void UpdateDistortionMap(float zoom, float focus)
    {
        LensTableUpdateDistortionMap(zoom, focus, distortionMap.GetNativeTextureID());
    }

    public void UpdateProjection()
    {
        LensTableProjection(projectionMatrix.Handle.AddrOfPinnedObject());
    }

   
    public float FieldOfView(float fov_normalized)
    {
        return Lerp(fov_normalized, 0.0f, 1.0f, fovMin, fovMax);
    }

    public float FieldOfView()
    {
        return LensTableFov();
    }



    static float Lerp(float x, float x0, float x1, float y0, float y1)
    {
        if ((x1 - x0) == 0)
            return (y0 + y1) / 2;
        return y0 + (x - x0) * (y1 - y0) / (x1 - x0);
    }

}


public class LensProjection
{
    private float[] data = null;
    public GCHandle Handle;

    public LensProjection()
    {
        data = new float[16];
        Handle = GCHandle.Alloc(data, GCHandleType.Pinned);
    }

    public LensProjection(float[] matrix_array_16)
    {
        data = matrix_array_16;
        Handle = GCHandle.Alloc(data, GCHandleType.Pinned);
    }

    ~LensProjection()
    {
        Handle.Free();
    }

    public void UpdateCamera(Camera cam)
    {
        cam.ResetProjectionMatrix();    // the reset is done to allow gizmo update
        cam.fieldOfView = Fovy;         // set fov (for gizmo update)
        // There is a number precision difference between ImageAspectRatio and the ratio between m11 and m00
        cam.aspect = Aspect;            // set aspect  (for gizmo update)

        Matrix4x4 p = cam.projectionMatrix;
        p[0, 2] = -data[8];             // set ccd shift on x
        p[1, 2] = -data[9];             // set ccd shift on y
        cam.projectionMatrix = p;
    }

    public Matrix4x4 Matrix
    {
        get
        {
            Matrix4x4 mat = Matrix4x4.identity;
            for (int i = 0; i < 16; ++i)
                mat[i] = data[i];

            mat[1, 1] = -data[5];
            mat[0, 2] = -data[8];
            mat[1, 2] = -data[9];
            mat[2, 2] = -data[10];
            mat[3, 2] = -data[11];

            return mat;
        }
    }

    public float[] DataArray
    {
        get { return data; }
        set { data = value; }
    }

    public float Fovy
    {
        get 
        {
            float m = Mathf.Pow(-data[5], -1.0f);
            return Mathf.Atan(m) * Mathf.Rad2Deg * 2.0f;
        }
    }

    public float Aspect
    {
        get
        {
            return Mathf.Abs(data[5] / data[0]);
        }
    }

    public Vector2 ShiftCCD
    {
        get 
        { 
            return new Vector2(-data[8], -data[9]); 
        }
    }

    public void ApplyCcdShift(Camera cam)
    {
        Matrix4x4 p = cam.projectionMatrix;
        p[0, 2] = -data[8];             // set ccd shift on x
        p[1, 2] = -data[9];             // set ccd shift on y
        cam.projectionMatrix = p;
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


public class LensDistortionData
{
    private float[] data = null;
    public GCHandle Handle;

    public LensDistortionData(int size)
    {
        this.data = new float[size];
        this.Handle = GCHandle.Alloc(data, GCHandleType.Pinned);
    }

    ~LensDistortionData()
    {
        this.Handle.Free();
    }

    public int Length
    {
        get { return data.Length; }
    }
}

