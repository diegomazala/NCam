using UnityEngine;
using System;

[ExecuteInEditMode]
[RequireComponent(typeof(Camera))]
[AddComponentMenu("Image Effects/Blit Target Texture")]
public class BlitTargetTexture : MonoBehaviour 
{
    public Shader shader;
    public Camera targetCamera = null;
    public float fixAspectRatio = 1;


    void OnRenderImage(RenderTexture source, RenderTexture destination)
    {
        material.SetFloat("_FixAspect", fixAspectRatio);
        material.SetTexture("_MainTex", targetCamera.targetTexture);
        Graphics.Blit(source, destination, material);

        RenderTexture.active = destination;
    }

    void OnEnable()
    {
        // Try to find the shader
        if (!shader)
            shader = Shader.Find("Diem/BlitTexture");


        //if (!targetCamera || !targetCamera.targetTexture)
        if (!targetCamera)
        {
            Debug.LogError("Missing reference to a target camera or the target camera does not render to a texture.");
            enabled = false;
            return;
        }

        // Disable if we don't support image effects
        if (!SystemInfo.supportsImageEffects)
        {
            enabled = false;
            return;
        }

        // Disable the image effect if the shader can't
        // run on the users graphics card
        if (!shader || !shader.isSupported)
            enabled = false;
    }

    protected virtual void OnDisable()
    {
        if (m_Material)
        {
            DestroyImmediate(m_Material);
        }
    }

    private Material m_Material;
    protected Material material
    {
        get
        {
            if (m_Material == null)
            {
                m_Material = new Material(shader);
                m_Material.hideFlags = HideFlags.HideAndDontSave;
            }
            return m_Material;
        }
    }
}
