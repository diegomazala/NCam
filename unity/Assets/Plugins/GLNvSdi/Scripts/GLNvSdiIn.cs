using UnityEngine;
using System.Collections;

[AddComponentMenu("Diem/Video In SDI - GLNvSdiIn")]
public class GLNvSdiIn : MonoBehaviour 
{
    public const int MAX_COUNT = 8;

    public int ringBufferSizeInFrames = 2;

    public bool captureFields = true;

    public Material[] sdiMaterials = { null, null, null, null, null, null, null, null };

    private RenderTexture[] sdiTexture = { null, null, null, null, null, null, null, null };

    private bool sdiActive = false;
    

    void Awake()
    {
        sdiActive = false;

        if (!SystemInfo.graphicsDeviceVersion.Contains("OpenGL"))
        {
            this.enabled = false;
            return;
        }
    }



    IEnumerator Start()
    {
        yield return new WaitForEndOfFrame();
        sdiActive = SetupSdiInput();

        if (sdiActive)
            yield return StartCoroutine("CallPluginAtEndOfFrames");
        else
            yield return null;
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
            }
            else
            {
                //Debug.LogError("Capture fail");
            }
        }
    }



    



    bool SetupSdiInput()
    {
        UtyGLNvSdi.SdiSetupLogFile();
        UtyGLNvSdi.SdiSetCurrentDC();
        UtyGLNvSdi.SdiSetCurrentGLRC();

        if (!UtyGLNvSdi.SdiInputInitialize())
        {
            UnityEngine.Debug.LogError("GLNvSdi_Plugin: " + UtyGLNvSdi.SdiGetLog());
            return false;
        }

        UtyGLNvSdi.SdiInputSetGlobalOptions(ringBufferSizeInFrames);

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
            CreateTextures(8, UtyGLNvSdi.SdiInputWidth(), UtyGLNvSdi.SdiInputHeight() / 2);
        else
            CreateTextures(4, UtyGLNvSdi.SdiInputWidth(), UtyGLNvSdi.SdiInputHeight());


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



        if (!UtyGLNvSdi.SdiInputStart())
        {
            UnityEngine.Debug.LogError("GLNvSdi_Plugin: " + UtyGLNvSdi.SdiGetLog());
            return false;
        }

        return true;
    }


    void CleanupSdiInput()
    {
        if (sdiActive)
        {
            sdiActive = false;

            UtyGLNvSdi.SdiSetUtyDC();
            UtyGLNvSdi.SdiSetUtyGLRC();
            UtyGLNvSdi.SdiMakeCurrent();

            StopCoroutine("CallPluginAtEndOfFrames");

            UtyGLNvSdi.SdiInputStop();

            UtyGLNvSdi.SdiAncCleanupInput();

            if (captureFields)
                UtyGLNvSdi.SdiInputUnbindVideoTextureField();
            else
                UtyGLNvSdi.SdiInputUnbindVideoTextureFrame();

            UtyGLNvSdi.SdiInputCleanupGL();

            UtyGLNvSdi.SdiInputUninitialize();

            UtyGLNvSdi.SdiMakeUtyCurrent();

            DestroyTextures();
        }
    }


    void CreateTextures(int count, int texWidth, int texHeight)
    {
        for (int i = 0; i < count; ++i)
        {
            sdiTexture[i] = new RenderTexture(texWidth, texHeight, 32, RenderTextureFormat.ARGB32);
            sdiTexture[i].name = name;
            sdiTexture[i].isPowerOfTwo = false;
            sdiTexture[i].wrapMode = TextureWrapMode.Clamp;
            sdiTexture[i].Create();

            UtyGLNvSdi.SdiInputSetTexture(i, sdiTexture[i].GetNativeTextureID());
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




    void OnDisable()
    {
        CleanupSdiInput();
    }

}
