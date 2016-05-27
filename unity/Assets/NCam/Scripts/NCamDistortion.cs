using UnityEngine;


[AddComponentMenu("NCam/Distortion")]
[RequireComponent(typeof(Camera))]
public class NCamDistortion : MonoBehaviour
{
    /// Provides a shader property that is set in the inspector
    /// and a material instantiated from the shader
    public Shader shader;
    private Material m_Material;

    [HideInInspector]
    public NCam ncam = null;

	
	void OnRenderImage (RenderTexture source, RenderTexture destination)
	{
        if (ncam != null)
        {
            material.SetTexture("DistortTex", ncam.distortionMap);
            material.SetFloat("DistortUScale", (ncam.distortionMapSize.x - 1.0f) / ncam.distortionMapSize.x);
            material.SetFloat("DistortUOffset", 0.5f / ncam.distortionMapSize.x);
            material.SetFloat("DistortVScale", (ncam.distortionMapSize.y - 1.0f) / ncam.distortionMapSize.y);
            material.SetFloat("DistortVOffset", 0.5f / ncam.distortionMapSize.y);

            Graphics.Blit(source, destination, material);
        }
        else
        {
            enabled = false;
        }
	}

    

    void Start()
    {
        if (ncam == null)
        {
            enabled = false;
            return;
        }

        if (shader == null)
            shader = Shader.Find("NCam/Distortion");

        if (!SystemInfo.supportsRenderTextures)
        {
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

    protected virtual void OnDisable()
    {
        if (m_Material)
        {
            DestroyImmediate(m_Material);
        }
    }

}
