using UnityEngine;
using System.Collections;
using System.Runtime.InteropServices;

[AddComponentMenu("Diem/Video IO SDI - GLNvSdiIO")]
public class GLNvSdiIO : MonoBehaviour
{
    public const int MAX_COUNT = 8;

    public bool captureFields = true;

    public bool useVideoAsBackground = true;

    public GLNvSdiOptions options;

    public Material[] sdiMaterials = { null, null, null, null, null, null, null, null };

    private RenderTexture[] sdiTexture = { null, null, null, null, null, null, null, null };
        
    public Camera[] m_Camera = { null, null, null, null };

    private bool sdiActive = false;

    private int[] timeCodeData;
    private GCHandle timeCodeHandle;



    public bool showGUI = false;


    void OnEnable()
    {
        sdiActive = false;

        if (!SystemInfo.graphicsDeviceVersion.Contains("OpenGL"))
        {
            this.enabled = false;
            return;
        }

        timeCodeData = new int[8] { 0, 0, 0, 0, 0, 0, 0, 0 };
        timeCodeHandle = GCHandle.Alloc(timeCodeData, GCHandleType.Pinned);
    }

    void OnDisable()
    {
        timeCodeHandle.Free();
    }

    IEnumerator Start()
    {
        yield return new WaitForEndOfFrame();


#if !UNITY_EDITOR
        if (!GLNvSdiOptions.ReadXml(UtyGLNvSdi.sdiConfigFolder + @"Sdi.xml", ref options))
        {
            System.IO.DirectoryInfo sdiDir = new System.IO.DirectoryInfo(UtyGLNvSdi.sdiConfigFolder);
            if (!sdiDir.Exists)
                sdiDir.Create();
            GLNvSdiOptions.WriteXml(UtyGLNvSdi.sdiConfigFolder + @"Sdi.xml", options);
        }
#endif

        sdiActive = SetupSdiIO();
        if (sdiActive)
        {
            yield return StartCoroutine("CallPluginAtEndOfFrames");
        }
        else
        {
            enabled = false;
            yield return null;
        }
    }

    
    private IEnumerator CallPluginAtEndOfFrames()
    {
        while (sdiActive)
        {
            // Wait until all frame rendering is done
            yield return new WaitForEndOfFrame();
               

            if (UtyGLNvSdi.SdiInputCaptureVideo() != (uint)SdiReturn.GL_FAILURE_NV)
            {
                UtyGLNvSdi.SdiAncCapture();
                UtyGLNvSdi.SdiAncGetTimeCode(timeCodeHandle.AddrOfPinnedObject(), 0);
            }
            else
            {
                //Debug.LogError("Capture fail");
            }


            if (guiText != null)
            {
                guiText.text = timeCodeData[0].ToString() + timeCodeData[1].ToString() + ':' +
                                timeCodeData[2].ToString() + timeCodeData[3].ToString() + ':' +
                                timeCodeData[4].ToString() + timeCodeData[5].ToString() + ':' +
                                timeCodeData[6].ToString() + timeCodeData[7].ToString();
            }

            UtyGLNvSdi.SdiAncPresent();
            UtyGLNvSdi.SdiOutputPresentFrame();

            // Issue a plugin event with arbitrary integer identifier.
            // The plugin can distinguish between different
            // things it needs to do based on this ID.
            // For our simple plugin, it does not matter which ID we pass here.
            //GL.IssuePluginEvent(1);
        }
    }


    bool SetupSdiIO()
    {
        UtyGLNvSdi.SdiSetupLogFile();
        UtyGLNvSdi.SdiSetCurrentDC();
        UtyGLNvSdi.SdiSetCurrentGLRC();
        
        if (!UtyGLNvSdi.SdiInputInitialize())
        {
            UnityEngine.Debug.LogError("GLNvSdi_Plugin: " + UtyGLNvSdi.SdiGetLog());
            return false;
        }

        if (!UtyGLNvSdi.SdiMakeCurrent())
        {
            UnityEngine.Debug.LogError("GLNvSdi_Plugin: " + UtyGLNvSdi.SdiGetLog());
            return false;
        }

        if (!UtyGLNvSdi.SdiOutputInitialize())
        {
            UnityEngine.Debug.LogError("GLNvSdi_Plugin: " + UtyGLNvSdi.SdiGetLog());
            return false;
        }

        UtyGLNvSdi.SdiInputSetGlobalOptions(options.inputRingBufferSize);

        if (!UtyGLNvSdi.SdiInputSetupDevices())
        {
            UnityEngine.Debug.LogError("GLNvSdi_Plugin: " + UtyGLNvSdi.SdiGetLog());
            return false;
        }

        if (!UtyGLNvSdi.SdiMakeCurrent())
        {
            UnityEngine.Debug.LogError("GLNvSdi_Plugin: " + UtyGLNvSdi.SdiGetLog());
            return false;
        }


        if (captureFields)
            CreateSdiInputTextures(8, UtyGLNvSdi.SdiInputWidth(), UtyGLNvSdi.SdiInputHeight() / 2);
        else
            CreateSdiInputTextures(4, UtyGLNvSdi.SdiInputWidth(), UtyGLNvSdi.SdiInputHeight());


        UtyGLNvSdi.SdiOutputSetGlobalOptions();
        UtyGLNvSdi.SdiOutputSetVideoFormat(options.videoFormat, options.syncSource, options.hDelay, options.vDelay, options.dualOutput, options.flipQueueLength);

        if (!UtyGLNvSdi.SdiOutputSetupDevices())
        {
            UnityEngine.Debug.LogError("GLNvSdi_Plugin: " + UtyGLNvSdi.SdiGetLog());
            return false;
        }


        if (!UtyGLNvSdi.SdiMakeCurrent())
        {
            UnityEngine.Debug.LogError("GLNvSdi_Plugin: " + UtyGLNvSdi.SdiGetLog());
            return false;
        }


#if passthru
        UtyGLNvSdi.SdiOutputSetTexture(0, sdiTexture[0].GetNativeTextureID());
        UtyGLNvSdi.SdiOutputSetTexture(1, sdiTexture[1].GetNativeTextureID());
#else
        int texWidth = 1920;
        int texHeight = 1080;
        bool interlaced = true;
        float aspect = 1.0f;

        UtyGLNvSdi.GetSizeFromVideoFormat(options.videoFormat, ref texWidth, ref texHeight, ref aspect, ref interlaced);
        SetupOutputTextures(texWidth, texHeight, aspect, interlaced, options.dualOutput);
#endif   

        if (!UtyGLNvSdi.SdiInputSetupGL())
        {
            UnityEngine.Debug.LogError("GLNvSdi_Plugin: " + UtyGLNvSdi.SdiGetLog());
            return false;
        }

        if (captureFields)
        {
            if (!UtyGLNvSdi.SdiInputBindVideoTextureField())
            {
                UnityEngine.Debug.LogError("GLNvSdi_Plugin: " + UtyGLNvSdi.SdiGetLog());
                return false;
            }
        }
        else
        {
            if (!UtyGLNvSdi.SdiInputBindVideoTextureFrame())
            {
                UnityEngine.Debug.LogError("GLNvSdi_Plugin: " + UtyGLNvSdi.SdiGetLog());
                return false;
            }
        }

        if (!UtyGLNvSdi.SdiAncSetupInput())
        {
            UnityEngine.Debug.LogError("GLNvSdi_Plugin: " + UtyGLNvSdi.SdiGetLog());
            return false;
        }
        
        if (!UtyGLNvSdi.SdiOutputSetupGL())
        {
            UnityEngine.Debug.LogError("GLNvSdi_Plugin: " + UtyGLNvSdi.SdiGetLog());
            return false;
        }

        if (!UtyGLNvSdi.SdiOutputBindVideo())
        {
            UnityEngine.Debug.LogError("GLNvSdi_Plugin: " + UtyGLNvSdi.SdiGetLog());
            return false;
        }
       

        if (!UtyGLNvSdi.SdiInputStart())
        {
            UnityEngine.Debug.LogError("GLNvSdi_Plugin: " + UtyGLNvSdi.SdiGetLog());
            return false;
        }

        if (!UtyGLNvSdi.SdiOutputStart())
        {
            UnityEngine.Debug.LogError("GLNvSdi_Plugin: " + UtyGLNvSdi.SdiGetLog());
            return false;
        }

        return true;
    }

    bool SetupOutputTextures(int texWidth, int texHeight, float aspect, bool interlaced, bool dualOutput)
    {

        // Check the count of cameras. The plugin support until two cameras
        if (m_Camera.Length > 4)
        {
            UnityEngine.Debug.LogWarning("GLNvSdi plugin does not support more than four cameras.");
        }

        // Disable all cameras
        for (int i = 0; i < m_Camera.Length; ++i)
            m_Camera[i].enabled = false;

        // Verify the amount of render textures needed
        int lTexCount = 0;
        if (options.dualOutput)
            lTexCount = 2;
        else
            lTexCount = 1;

        if (interlaced)
            lTexCount *= 2;

        // If dual progressive output, change the order of cameras. So, the first two are active
        if (options.dualOutput && !interlaced)     // dual output progressive
        {
            Camera tmp = m_Camera[1];
            m_Camera[1] = m_Camera[2];
            m_Camera[2] = tmp;

            GLNvSdiRenderTexture rt1 = m_Camera[1].gameObject.GetComponent(typeof(GLNvSdiRenderTexture)) as GLNvSdiRenderTexture;
            GLNvSdiRenderTexture rt2 = m_Camera[2].gameObject.GetComponent(typeof(GLNvSdiRenderTexture)) as GLNvSdiRenderTexture;
            if (rt1 != null && rt2 != null)
            {
                int tmp_index = rt1.sdiTextureIndex;
                rt1.sdiTextureIndex = rt2.sdiTextureIndex;
                rt2.sdiTextureIndex = tmp_index;
            }
            else
            {
                UnityEngine.Debug.LogError("Error: Could not find GLNvSdiRenderTexture when switching camera parameters");
                return false;
            }
        }


        // Setting the cameras and directing the render to texture
        for (int i = 0; i < lTexCount; ++i)
        {
            if (m_Camera[i] != null)
            {
                m_Camera[i].aspect = aspect;
                m_Camera[i].enabled = true;

                GLNvSdiRenderTexture sdiTex = m_Camera[i].gameObject.GetComponent(typeof(GLNvSdiRenderTexture)) as GLNvSdiRenderTexture;
                if (sdiTex != null)
                {
                    sdiTex.CreateSdiTexture(texWidth, texHeight, 32, false, "SdiRenderTex_" + m_Camera[i].name);

                    if (useVideoAsBackground)
                    {
                        if (captureFields)
                            sdiTex.backgroundTex = sdiTexture[i];
                        else
                            sdiTex.backgroundTex = sdiTexture[i / 2];
                    }

                    UtyGLNvSdi.SdiOutputSetTexture(i, sdiTex.sdiRenderTarget.GetNativeTextureID());
                }
            }
            else
            {
                UnityEngine.Debug.LogError(m_Camera[i].name + " is null");
                return false;
            }
        }

        return true;
    }


    void OnApplicationQuit()
    {
        if (sdiActive)
        {
            sdiActive = false;

            UtyGLNvSdi.SdiSetUtyDC();
            UtyGLNvSdi.SdiSetUtyGLRC();
            UtyGLNvSdi.SdiMakeCurrent();

            UtyGLNvSdi.SdiInputStop();
            UtyGLNvSdi.SdiOutputStop();

            UtyGLNvSdi.SdiAncCleanupInput();

            if (captureFields)
                UtyGLNvSdi.SdiInputUnbindVideoTextureField();
            else
                UtyGLNvSdi.SdiInputUnbindVideoTextureFrame();

            UtyGLNvSdi.SdiInputCleanupGL();

            UtyGLNvSdi.SdiOutputUnbindVideo();
            UtyGLNvSdi.SdiOutputCleanupGL();

            UtyGLNvSdi.SdiInputUninitialize();

            UtyGLNvSdi.SdiOutputCleanupDevices();

            UtyGLNvSdi.SdiMakeUtyCurrent();

            DestroyTextures();
        }
                
    }


    void CreateSdiInputTextures(int count, int texWidth, int texHeight)
    {
        // Remember: if input videoFormat == SdiVideoFormat.SD_487I_59_94, that is, texHeight == 243
        // the upper field has one line more than the lower field. 
        // UpperField = 244; LowerField = 243

        for (int i = 0; i < count; ++i)
        {
            if (captureFields && texHeight == 243 && i % 2 == 0)
                sdiTexture[i] = new RenderTexture(texWidth, texHeight + 1, 32, RenderTextureFormat.ARGB32);
            else
                sdiTexture[i] = new RenderTexture(texWidth, texHeight, 32, RenderTextureFormat.ARGB32);
            sdiTexture[i].name = name;
            sdiTexture[i].isPowerOfTwo = false;
            sdiTexture[i].useMipMap = false;
            sdiTexture[i].filterMode = FilterMode.Trilinear;
            sdiTexture[i].wrapMode = TextureWrapMode.Clamp;
            sdiTexture[i].Create();

            UtyGLNvSdi.SdiInputSetTexture(i, sdiTexture[i].GetNativeTextureID());

            // attach to texture only the existent video inputs
            int multiplier = captureFields ? 2 : 1;
            if (i < UtyGLNvSdi.SdiInputVideoCount() * multiplier)
                sdiMaterials[i].mainTexture = sdiTexture[i];
        }
    }


    void DestroyTextures()
    {
        for (int i = 0; i < MAX_COUNT; ++i)
        {
            if (sdiTexture[i] != null && sdiTexture[i].IsCreated())
            {
                sdiTexture[i].Release();
                sdiTexture[i] = null;
            }
        }
    }

    void Update()
    {
        if (Input.GetKeyDown(KeyCode.V))
            showGUI = !showGUI;
    }

    private Rect windowRect = new Rect(10, 10, 250, 200);
    private int windowId = 0;
    void OnGUI()
    {
        if (!showGUI)
            return;

        if (windowId == 0)
            Random.Range(1, 9999);
        windowRect = GUILayout.Window(windowId, windowRect, BuildWindow, "Sdi [Key 'V']");
    }


    void BuildWindow(int windowId)
    {
        GUILayout.BeginHorizontal("box");
        {
            GUILayout.Label("Video Format: ");
            GUILayout.Label(this.options.videoFormat.ToString(), "box");
        }
        GUILayout.EndHorizontal();

        GUILayout.BeginHorizontal("box");
        {
            GUILayout.Label("Sync Source: ");
            GUILayout.Label(this.options.syncSource.ToString(), "box");
        }
        GUILayout.EndHorizontal();

        GUILayout.BeginHorizontal("box");
        {
            GUILayout.Label("Input Ring Buffer: ");
            GUILayout.Label(this.options.inputRingBufferSize.ToString(), "box");
        }
        GUILayout.EndHorizontal();

        GUILayout.BeginHorizontal("box");
        {
            GUILayout.Label("Gvi Time: ");
            GUILayout.Label(UtyGLNvSdi.SdiInputGviTime().ToString(), "box");
        }
        GUILayout.EndHorizontal();

        GUILayout.BeginHorizontal("box");
        {
            if (GUILayout.Button("Reset"))
                UtyGLNvSdi.SdiInputResetDroppedFramesCount();
            GUILayout.Label("Dropped Frames Count: ");
            GUILayout.Label(UtyGLNvSdi.SdiInputDroppedFramesCount().ToString(), "box");
        }
        GUILayout.EndHorizontal();

        GUILayout.BeginHorizontal("box");
        {
            GUILayout.Label("Output Flip Queue Length: ");
            GUILayout.Label(this.options.flipQueueLength.ToString(), "box");
        }
        GUILayout.EndHorizontal();

        GUILayout.BeginHorizontal("box");
        {
            if (GUILayout.Button("Reset"))
                UtyGLNvSdi.SdiOutputResetDuplicatedFramesCount();
            GUILayout.Label("Duplicated Frames Count: ");
            GUILayout.Label(UtyGLNvSdi.SdiOutputDuplicatedFramesCount().ToString(), "box");
        }
        GUILayout.EndHorizontal();

        GUILayout.BeginHorizontal("box");
        {
            GUILayout.Label("Dual Output: ");
            GUILayout.Label(this.options.dualOutput.ToString(), "box");
        }
        GUILayout.EndHorizontal();

        GUILayout.BeginHorizontal("box");
        {
            GUILayout.Label("Horizontal Delay: ");
            GUILayout.Label(this.options.hDelay.ToString(), "box");
        }
        GUILayout.EndHorizontal();

        GUILayout.BeginHorizontal("box");
        {
            GUILayout.Label("Vertical Delay: ");
            GUILayout.Label(this.options.vDelay.ToString(), "box");
        }
        GUILayout.EndHorizontal();
    }
}
