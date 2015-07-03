using UnityEngine;
using System;



[RequireComponent(typeof(Camera))]
public class LensDistortionUVMap : MonoBehaviour
{
    [HideInInspector]
    public RenderTexture lensMap = null;
    [HideInInspector]
    public RenderTexture distortionMap = null;

    private RenderTexture texel = null;
    private Vector4 texCoord = Vector4.zero;
    private Color texColor = Color.black;


    public Shader texelShader = null;
    public Shader distortionMapShader = null;
    
    private Material texelMaterial = null;
    private Material distortionMaterial = null;

    private Texture2D texFeedback = null;


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
        texFeedback = new Texture2D(1, 1, TextureFormat.ARGB32, false);

        texel = new RenderTexture(1, 1, 8, RenderTextureFormat.ARGB32);
        texel.name = "LensTexel";
        texel.wrapMode = TextureWrapMode.Clamp;
        texel.filterMode = FilterMode.Point;
        texel.Create();  // Call Create() so it's actually uploaded to the GPU

        if (texelShader == null)
            texelShader = Shader.Find("Diem/TexelCoord");
        if (distortionMapShader == null)
            distortionMapShader = Shader.Find("Diem/LensDistortionUVMap");

        if (texelShader == null)
        {
            Debug.LogError("Could not find TexelCoord shader. Lens distortion will be disabled.");
            enabled = false;
            return;
        }
        if (distortionMapShader == null)
        {
            Debug.LogError("Could not find LensDistortionUVMap shader. Lens distortion will be disabled.");
            enabled = false;
            return;
        }

        texelMaterial = new Material(texelShader);
        distortionMaterial = new Material(distortionMapShader);
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
        //Graphics.Blit(lensMap, destination, material);            // Blit to default output
        //Graphics.Blit(distortionMap, destination, material);      // Blit to default output


        //=======================================================
        // Rendering using shader to compute the zoom, focus and fov values
        texelMaterial.SetVector("_TexCoord", texCoord);
        Graphics.Blit(lensMap, texel, texelMaterial);
        
        //=======================================================
        // Reading pixel color which is the zoom, focus and fov values
        texFeedback.ReadPixels(new Rect(Vector2.zero, Vector2.one), 0, 0);
        texColor = texFeedback.GetPixel(0, 0);

        //LensPlugin.LensTableUpdateSample(texCoord.x, texCoord.y, 0, texel.GetNativeTextureID());
        //texColor = new Color(LensPlugin.LensTableZoom(), LensPlugin.LensTableFocus(), LensPlugin.LensTableFov());

        RenderTexture.active = destination;

        //=======================================================
        // Rendering scene with distortion
        if (distortionMap != null)
        {
            float w = distortionMap.width;
            float h = distortionMap.height;
            distortionMaterial.SetTexture("DistortTex", distortionMap);
            distortionMaterial.SetFloat("DistortUScale", (w - 1.0f) / w);
            distortionMaterial.SetFloat("DistortUOffset", 0.5f / w);
            distortionMaterial.SetFloat("DistortVScale", (h - 1.0f) / h);
            distortionMaterial.SetFloat("DistortVOffset", 0.5f / h);

            Graphics.Blit(source, destination, distortionMaterial);
        }
    }
}
