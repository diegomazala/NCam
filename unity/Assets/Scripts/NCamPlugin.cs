using UnityEngine;
using System.Collections;
using System.Runtime.InteropServices;
using System.Xml;

public enum NCamRenderEvent
{
    Initialize,
    Update,
    UpdateDistortion,
    Uninitialize
};



public class NCamPlugin 
{
    //public const string DllName = "NCamWin";
    public const string DllName = "NCamQt";


    [DllImport(DllName)]
    public static extern uint NCamErrorCode();
    [DllImport(DllName)]
    public static extern void NCamResetError();
    [DllImport(DllName)]
    public static extern void NCamSetPacketType(bool cameraTracking, bool opticalParamters, bool distortMap);

    [DllImport(DllName)]
    public static extern void NCamCreate();
    [DllImport(DllName)]
    public static extern void NCamDestroy();

    [DllImport(DllName)]
    public static extern void NCamSetIpAddress(string ip_address, int port);
    [DllImport(DllName)]
    public static extern bool NCamOpen();
    [DllImport(DllName)]
    public static extern bool NCamIsOpen();
    [DllImport(DllName)]
    public static extern void NCamClose();
    [DllImport(DllName)]
    public static extern bool NCamUpdate();

    [DllImport(DllName)]
    public static extern double NCamFrameRate();
    [DllImport(DllName)]
    public static extern int  NCamFrameLostCount();
    [DllImport(DllName)]
    public static extern int NCamFieldLostCount();

    [DllImport(DllName)]
    public static extern void NCamFieldIndex(int field_index);

    [DllImport(DllName)]
    public static extern void NCamGetData(System.IntPtr doubleArray6_OpticalTimeCode,
                                        System.IntPtr doubleArray6_TrackingTimeCode,
                                        System.IntPtr doubleArray13_OpticalParameters,
                                        System.IntPtr doubleArray12_TrackingParamters,
                                        System.IntPtr doubleArray16_GLProjectionMatrix,
                                        System.IntPtr doubleArray16_GLModelViewMatrix,
                                        double near_plane, double far_plane);


    [DllImport(DllName)]
    public static extern void NCamGLMatrices(System.IntPtr doubleArray16_GLProjectionMatrix,
                                        System.IntPtr doubleArray16_GLModelViewMatrix,
                                        double near_plane, double far_plane);

    [DllImport(DllName)]
    public static extern void NCamGLProjectionMatrix(System.IntPtr doubleArray16_GLProjectionMatrix,
                                                    double near_plane, double far_plane);

    [DllImport(DllName)]
    public static extern uint NCamTrackingTimeCode(System.IntPtr doubleArray6);
    [DllImport(DllName)]
    public static extern uint NCamOpticalTimeCode(System.IntPtr doubleArray6);

    [DllImport(DllName)]
    public static extern void NCamEncoderParameters(System.IntPtr doubleArray6);


    [DllImport(DllName)]
    public static extern double NCamFovHorizontal();
    [DllImport(DllName)]
    public static extern double NCamFovVertical();

    [DllImport(DllName)]
    public static extern void NCamOpticalParameters(System.IntPtr doubleArray13);
    [DllImport(DllName)]
    public static extern void NCamTrackingParameters(System.IntPtr doubleArray12);

    [DllImport(DllName)]
    public static extern void NCamGLModelViewMatrix(System.IntPtr doubleArray16);

    [DllImport(DllName)]
    public static extern bool NCamSetDistortMapPtr(System.IntPtr texPtr);
    [DllImport(DllName)]
    public static extern bool NCamUpdateDistortMap();
    [DllImport(DllName)]
    public static extern int NCamDistortMapWidth();
    [DllImport(DllName)]
    public static extern int NCamDistortMapHeight();
    [DllImport(DllName)]
    public static extern System.IntPtr GetNCamRenderEventFunc();

    public static ErrorCodeEnum ErrorCode()
    {
        return (ErrorCodeEnum)NCamErrorCode();
    }

    public enum ErrorCodeEnum
    {
        None,
        StartStreaming,
        StopStreaming,
        DoStreaming
    };
}


public class NCamMatrix
{
	private double[] data = null;
	public GCHandle Handle;

	public NCamMatrix()
	{
		data = new double[16];
		Handle = GCHandle.Alloc(data, GCHandleType.Pinned);
	}

	public NCamMatrix(double[] matrix_array_16)
	{
		data = matrix_array_16;
		Handle = GCHandle.Alloc(data, GCHandleType.Pinned);
	}

	~NCamMatrix()
	{
		Handle.Free();
	}

	public Matrix4x4 Matrix
	{
		get
		{
			Matrix4x4 mat = Matrix4x4.identity;
			for (int i = 0; i < 16; ++i)
				mat[i] = (float)data[i];
			return mat;
		}
	}

    public double[] DataArray
    {
        get { return data; }
        set { data = value; }
    }
}


public class NCamTimeCode
{
	private uint[] data = null;
	public GCHandle Handle;

	public NCamTimeCode()
	{
        data = new uint[6] { 0, 0, 0, 0, 0, 0 };
		Handle = GCHandle.Alloc(data, GCHandleType.Pinned);
	}

	public NCamTimeCode(uint[] time_code_array_6)
	{
        data = time_code_array_6;
		Handle = GCHandle.Alloc(data, GCHandleType.Pinned);
	}

	~NCamTimeCode()
	{
		Handle.Free();
	}

    public uint[] DataArray
    {
        get { return data; }
        set { data = value; }
    }

	public uint Hour
	{
		get { return data[0]; }
		set { data[0] = value; }
	}

	public uint Minute
	{
		get { return data[1]; }
		set { data[1] = value; }
	}

	public uint Second
	{
		get { return data[2]; }
		set { data[2] = value; }
	}

	public uint Frame
	{
		get { return data[3]; }
		set { data[3] = value; }
	}

	public uint DroppedFrame
	{
		get { return data[4]; }
		set { data[4] = value; }
	}

    public uint Time
    {
        get { return data[5]; }
        set { data[5] = value; }
    }

	public override string ToString()
	{
		string str = data[0].ToString("00");
		for(int i = 1; i < 4; ++i)
			str += ':' + data[i].ToString("00");
		return str;
	}

}


public class NCamOptical
{
    private double[] data = null;
	public GCHandle Handle;
    public NCamTimeCode TimeCode;

	public NCamOptical()
	{
        this.data = new double[13];
        this.Handle = GCHandle.Alloc(data, GCHandleType.Pinned);
        this.TimeCode = new NCamTimeCode();
	}

	public NCamOptical(double[] optical_data_array_13)
	{
        this.data = optical_data_array_13;
        this.Handle = GCHandle.Alloc(data, GCHandleType.Pinned);
        this.TimeCode = new NCamTimeCode();
	}

    ~NCamOptical()
	{
		this.Handle.Free();
	}

    public double[] DataArray
    {
        get { return this.data; }
        set { this.data = value; }
    }

    public float FieldOfViewVertical
    {
        get { return (float)this.data[1]; }
    }

    public Vector2 ProjectionCenter
    {
        get { return new Vector2((float)this.data[2], (float)this.data[3]); }
    }

    public Vector2 ImageSensorSize
    {
        get { return new Vector2((float)this.data[4], (float)this.data[5]); }
    }

    public Vector2 ImageResolution
    {
        get { return new Vector2((float)this.data[6], (float)this.data[7]); }
    }

    public float ImageAspectRatio
    {
        get { return (float)this.data[8]; }
    }
}



public class NCamTracking
{
    private double[] data = null;
    public GCHandle Handle;
    public NCamTimeCode TimeCode;

    public NCamTracking()
    {
        this.data = new double[12];
        this.Handle = GCHandle.Alloc(data, GCHandleType.Pinned);
        this.TimeCode = new NCamTimeCode();
    }

    public NCamTracking(double[] tracking_data_array_12)
    {
        this.data = tracking_data_array_12;
        this.Handle = GCHandle.Alloc(data, GCHandleType.Pinned);
        this.TimeCode = new NCamTimeCode();
    }

    ~NCamTracking()
    {
        this.Handle.Free();
    }

    public double[] DataArray
    {
        get { return this.data; }
        set { this.data = value; }
    }
       
}

[System.Serializable]
public class NCamEncoder
{
    public enum EParameter
    {
        ZoomNormalized,
        ZoomMapped,
        FocusNormalized,
        FocusMapped,
        IrisNormalized,
        IrisMapped, 
        ETotal
    }

    public double[] data = null;
    public GCHandle Handle;

    public NCamEncoder()
    {
        this.data = new double[(int)EParameter.ETotal];
        this.Handle = GCHandle.Alloc(data, GCHandleType.Pinned);
    }

    public NCamEncoder(double[] encoder_data_array_6)
    {
        this.data = encoder_data_array_6;
        this.Handle = GCHandle.Alloc(data, GCHandleType.Pinned);
    }

    ~NCamEncoder()
    {
        this.Handle.Free();
    }

    public System.IntPtr Ptr()
    {
        return Handle.AddrOfPinnedObject();
    }

    public double[] DataArray
    {
        get { return this.data; }
        set { this.data = value; }
    }

    public double GetValue(NCamEncoder.EParameter param)
    {
        return this.data[(int)param];
    }

}


public class NCamData
{
	public NCamTimeCode TrackingTimeCode;
	public NCamTimeCode OpticalTimeCode;
	public float Fov;
	public float AspectRatio;
	public Vector2 ImageResolution;
	public Vector2 ImageSensorSize;
	public Vector2 ProjectionCenter;
	public Matrix4x4 GLProjection;
	public Matrix4x4 GLModelView;
	public Vector2 DistortMapSize;

	public NCamData(NCamTimeCode tracking_time_code, NCamTimeCode optical_time_code, float fov_y, float aspect_ratio,
					Vector2 img_resolution, Vector2 img_sensor_size, Vector2 projection_center, Vector2 distort_map_size,
					Matrix4x4 gl_prj, Matrix4x4 gl_mdv)
	{
		TrackingTimeCode = tracking_time_code;
		OpticalTimeCode = optical_time_code;
		Fov = fov_y;
		AspectRatio = aspect_ratio;
		ImageResolution = img_resolution;
		ImageSensorSize = img_sensor_size;
		ProjectionCenter = projection_center;
		DistortMapSize = distort_map_size;
		GLProjection = gl_prj;
		GLModelView = gl_mdv;
	}
}


