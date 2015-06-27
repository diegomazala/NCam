using UnityEngine;
using System;



[RequireComponent(typeof(Camera))]
public class LensDistortion : ImageEffectBase
{
    public RenderTexture distortionMap = null;

    [HideInInspector]
    public RenderTexture lensMap = null;

    private RenderTexture texel = null;
    private Vector4 texCoord = Vector4.zero;
    private Color texColor = Color.black;

    public float Zoom
    {
        get { return texColor.r; }
        set { texCoord.x = value; }
    }

    public float Focus
    {
        get { return texColor.g; }
        set { texCoord.y = value; }
    }

    public float Fov
    {
        get { return texColor.b; }
        set { texCoord.z = value; }
    }

    public float Iris
    {
        get { return texColor.a; }
        set { texCoord.w = value; }
    }
   

    void OnEnable()
    {
        texel = new RenderTexture(1, 1, 8, RenderTextureFormat.ARGB32);
        texel.name = "LensTexel";
        texel.wrapMode = TextureWrapMode.Clamp;
        texel.filterMode = FilterMode.Point;
        texel.Create();  // Call Create() so it's actually uploaded to the GPU
    }


    void OnDisable()
    {
        if (texel != null)
        {
            if (RenderTexture.active == texel)
                RenderTexture.active = null;

            this.GetComponent<Camera>().targetTexture = null;
            if (texel != null)
            {
                DestroyImmediate(texel);
                texel = null;
            }
        }
    }


    void OnRenderImage(RenderTexture source, RenderTexture destination)
    {
        //Graphics.Blit(lensMap, destination, material);    // Blit to default output

#if true // debug

        material.SetVector("_TexCoord", texCoord);
        Graphics.Blit(lensMap, texel, material);


#if false   // using plugin to read sample
        //=======================================================
        //================== using plugin =======================
        if (lensTexel != null)
            LensPlugin.LensTableUpdateSample(texCoord.x, texCoord.y, texCoord.w, lensTexel.GetNativeTextureID());
        glzoom = LensPlugin.LensTableZoom();
        glfocus = LensPlugin.LensTableFocus();
        glfov = LensPlugin.LensTableFov();
        gliris = LensPlugin.LensTableIris();
        //=======================================================
#endif
        
        //=======================================================
        //================== using unity ========================
        Texture2D tex = new Texture2D(1, 1, TextureFormat.ARGB32, false);
        tex.ReadPixels(new Rect(Vector2.zero, Vector2.one), 0, 0);
        texColor = tex.GetPixel(0, 0);
        //=======================================================


        RenderTexture.active = destination;

#endif
    }

}
