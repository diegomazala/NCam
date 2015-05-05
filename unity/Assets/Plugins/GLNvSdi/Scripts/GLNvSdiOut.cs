/*
- Para que o script funcione corretamente, não esquecça de colocar as DLLs na pasta
   onde está o executável: GLNvSdi.dll 
- Além disso, não esqueça de executar o programa usando Opengl como api gráfica: game.exe -force-opengl
- Foi testado e está funcionando em HD1, HD Full e SD.
- O envio de SDI deve ser iniciado e finalizado somente uma vez por aplicação
- Caso tenha problemas com a qualidade da imagem, verifique o tamanho da RenderTexture. Elas são dimensionadas
   de acordo com a resolução escolhida: SD=720x486, HD=1920,1080. 
   Diego Mazala 
   Criado em 19 de julho de 2010
   Atualizado em 22 de novembro de 2012
*/
using UnityEngine;
using System;
using System.IO;
using System.Collections;
using System.Runtime.InteropServices;
using System.Diagnostics;
using System.Collections.Generic;
using System.Xml.Serialization;


[AddComponentMenu("Diem/Video Out SDI - GLNvSdiOut")]
public class GLNvSdiOut : MonoBehaviour
{
    public GLNvSdiOptions options;

    public Camera[] m_Camera = { null, null, null, null };

    private int m_TexWidth = 1920;	// HD=1920, SD=720
    private int m_TexHeight = 1080;	// HD=1080, SD=486


    private bool presentingFrame = false;
    private bool sdiActive = false;

    //string statusMsg;

    void Awake()
    {
        if (!SystemInfo.graphicsDeviceVersion.Contains("OpenGL"))
        {
            this.enabled = false;
            return;
        }
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
        
        presentingFrame = sdiActive = SetupOutput();
        
        if (sdiActive)
        {
            yield return StartCoroutine("CallPluginAtEndOfFrames");
        }
        else
        {
            UtyGLNvSdi.SdiMakeCurrent();
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

            if (presentingFrame)
            {
                UtyGLNvSdi.SdiMakeCurrent();
                UtyGLNvSdi.SdiOutputPresentFrame();

                // Issue a plugin event with arbitrary integer identifier.
                // The plugin can distinguish between different
                // things it needs to do based on this ID.
                // For our simple plugin, it does not matter which ID we pass here.
                //GL.IssuePluginEvent(1);
            }
        }
    }


    void OnApplicationQuit()
    {
        //this.WriteXml();
    }

    bool SetupOutputTextures()
    {
        float lAspect = 0;
        bool lIsInterlaced = false;
        UtyGLNvSdi.GetSizeFromVideoFormat(options.videoFormat, ref m_TexWidth, ref m_TexHeight, ref lAspect, ref lIsInterlaced);
        
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

        if (lIsInterlaced)
            lTexCount *= 2;

        // If dual progressive output, change the order of cameras. So, the first two are active
        if (options.dualOutput && !lIsInterlaced)     // dual output progressive
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
                m_Camera[i].aspect = lAspect;
                m_Camera[i].enabled = true;

                GLNvSdiRenderTexture sdiTex = m_Camera[i].gameObject.GetComponent(typeof(GLNvSdiRenderTexture)) as GLNvSdiRenderTexture;
                if (sdiTex.sdiRenderTarget == null)
                {
                    sdiTex.CreateSdiTexture(m_TexWidth, m_TexHeight, 32, false, "RenderTex_" + m_Camera[i].name);
                    UtyGLNvSdi.SdiOutputSetTexture(i, sdiTex.sdiRenderTarget.GetNativeTextureID());

                    if (sdiTex.sdiRenderTarget == null)
                    {
                        UnityEngine.Debug.LogError("Could not create SdiRenderTarget for GLNvSdiRenderTexture in " + m_Camera[i].name);
                        return false;
                    }
                }
                else
                {
                    UnityEngine.Debug.LogError("Could not find GLNvSdiRenderTexture in " + m_Camera[i].name);
                    return false;
                }
            }
            else
            {
                UnityEngine.Debug.LogError(m_Camera[i].name + " is null");
                return false;
            }
        }

        // If the options to invert fields is marked, we have to invert 
        // the textures id already sent to plugin
        if (options.invertFields)
        {
            UtyGLNvSdi.SdiOutputInvertFields(options.invertFields);
        }

        return true;
    }


    bool SetupOutput()
    {
        UtyGLNvSdi.SdiSetupLogFile();

        if (!SetupOutputTextures())
        {
            UnityEngine.Debug.LogError("GLNvSdi_Plugin could not setup sdi textures for output");
            return false;
        }
        
        UtyGLNvSdi.SdiSetCurrentDC();
        UtyGLNvSdi.SdiSetCurrentGLRC();

        if (!UtyGLNvSdi.SdiOutputInitialize())
        {
            UnityEngine.Debug.LogError("GLNvSdi_Plugin: " + UtyGLNvSdi.SdiGetLog());
            return false;
        }


        UtyGLNvSdi.SdiOutputSetGlobalOptions();
        UtyGLNvSdi.SdiOutputSetVideoFormat(options.videoFormat, options.syncSource, options.hDelay, options.vDelay, options.dualOutput, options.flipQueueLength);

        if (!UtyGLNvSdi.SdiOutputSetupDevices())
        {
            UnityEngine.Debug.LogError("GLNvSdi_Plugin: " + UtyGLNvSdi.SdiGetLog());
            return false;
        }

        UtyGLNvSdi.SdiMakeCurrent();
        if (!UtyGLNvSdi.SdiOutputSetupGL())
        {
            UnityEngine.Debug.LogError("GLNvSdi_Plugin: " + UtyGLNvSdi.SdiGetLog());
            UtyGLNvSdi.SdiOutputCleanupDevices();
            return false;
        }

        if (!UtyGLNvSdi.SdiOutputBindVideo())
        {
            UnityEngine.Debug.LogError("GLNvSdi_Plugin: " + UtyGLNvSdi.SdiGetLog());
            UtyGLNvSdi.SdiOutputCleanupDevices();
            return false;
        }

        if (!UtyGLNvSdi.SdiOutputStart())
        {
            UnityEngine.Debug.LogError("GLNvSdi_Plugin: " + UtyGLNvSdi.SdiGetLog());
            UtyGLNvSdi.SdiOutputUnbindVideo();
            UtyGLNvSdi.SdiOutputCleanupDevices();
            return false;
        }

        return true;
    }



    void OnDisable()
    {
        if (sdiActive)
        {
            UtyGLNvSdi.SdiSetUtyDC();
            UtyGLNvSdi.SdiSetUtyGLRC();
            UtyGLNvSdi.SdiMakeCurrent();

            StopCoroutine("CallPluginAtEndOfFrames");

            UtyGLNvSdi.SdiOutputStop();
            UtyGLNvSdi.SdiOutputUnbindVideo();
            UtyGLNvSdi.SdiOutputCleanupGL();
            UtyGLNvSdi.SdiOutputCleanupDevices();

            UtyGLNvSdi.SdiMakeUtyCurrent();

            sdiActive = false;
        }
    }


    void OnEnable()
    {
        if (!SystemInfo.graphicsDeviceVersion.Contains("OpenGL"))
        {
            this.enabled = false;
            return;
        }
    }


}

