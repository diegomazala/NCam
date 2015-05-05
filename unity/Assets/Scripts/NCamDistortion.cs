using UnityEngine;


[AddComponentMenu("Image Effects/NCam Distortion ")]
[RequireComponent(typeof(Camera))]
public class NCamDistortion : ImageEffectBase
{
    public int FieldIndex = 0;
    [HideInInspector]
    public NCam ncam = null;
    
	protected new void Start()
	{
        if (ncam == null)
        {
            enabled = false;
            return;
        }


		if(!SystemInfo.supportsRenderTextures)
		{
			enabled = false;
			return;
		}
		base.Start();
	}
	


	void OnRenderImage (RenderTexture source, RenderTexture destination)
	{
        if (ncam != null)
        {
            material.SetTexture("DistortTex", ncam.DistortMap[FieldIndex]);
            material.SetFloat("DistortUScale", (ncam.DistortMapSize.x - 1.0f) / ncam.DistortMapSize.x);
            material.SetFloat("DistortUOffset", 0.5f / ncam.DistortMapSize.x);
            material.SetFloat("DistortVScale", (ncam.DistortMapSize.y - 1.0f) / ncam.DistortMapSize.y);
            material.SetFloat("DistortVOffset", 0.5f / ncam.DistortMapSize.y);

            Graphics.Blit(source, destination, material);
        }
        else
        {
            enabled = false;
        }
	}


}
