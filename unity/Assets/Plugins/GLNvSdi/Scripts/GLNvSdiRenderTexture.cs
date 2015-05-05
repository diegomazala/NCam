using UnityEngine;
using System.Collections;


[RequireComponent(typeof(Camera))]
public class GLNvSdiRenderTexture : MonoBehaviour 
{
    public int sdiTextureIndex = 0;

    [HideInInspector]
    public  RenderTexture sdiRenderTarget = null;

    [HideInInspector]
    public Texture backgroundTex;

    [HideInInspector]
    public Material m_Material;


    void Awake()
    {
        CreateMaterial();
    }

    public void CreateSdiTexture(int w, int h, int depth, bool is_power_of_two, string name)
    {
        sdiRenderTarget = new RenderTexture(w, h, depth, RenderTextureFormat.ARGB32);
        sdiRenderTarget.name = name;
        sdiRenderTarget.isPowerOfTwo = is_power_of_two;
        sdiRenderTarget.useMipMap = false;
        sdiRenderTarget.filterMode = FilterMode.Trilinear;
        sdiRenderTarget.wrapMode = TextureWrapMode.Clamp;
        sdiRenderTarget.antiAliasing = 8;
        sdiRenderTarget.Create();

        this.camera.targetTexture = sdiRenderTarget;
    }
    

    void OnApplicationQuit()
    {
        if (sdiRenderTarget != null)
        {
            if (RenderTexture.active == sdiRenderTarget)
                RenderTexture.active = null;

            this.camera.targetTexture = null;
            if (sdiRenderTarget != null)
            {
                DestroyImmediate(sdiRenderTarget);
                sdiRenderTarget = null;
            }
        }
    }



    void OnRenderImage(RenderTexture source, RenderTexture destination)
    {
		if (backgroundTex != null)
		{
			material.SetTexture("_Bg", backgroundTex);
			Graphics.Blit(source, destination, material);
		}
		else
		{
			Graphics.Blit(source, destination);
		}
    }
	



    protected Material material 
    {
		get { return m_Material; } 
	}


    protected void CreateMaterial()
    {
        string shaderText =
                    "Shader \"BackgroundImage\" {" +
                    "Properties { _MainTex (\"Foreground (RGBA)\", 2D) = \"white\" " +
                    " _Bg (\"Background (RGB)\", 2D) = \"white\" }" +
                    "SubShader {" +
                    "	Pass {" +
                    "		SetTexture [_Bg] { combine texture }" +
                    "		SetTexture [_MainTex] { combine texture lerp (texture) previous }" +
                    "	}" +
                    " }" +
                    "}";
        m_Material = new Material(shaderText);
        m_Material.hideFlags = HideFlags.HideAndDontSave;
    }

}
