using UnityEngine;
using System;



[RequireComponent(typeof(Camera))]
public class LensDistortionUVMap : MonoBehaviour
{
    public RenderTexture distortionMap = null;
    public Shader distortionMapShader = null;
    private Material distortionMaterial = null;
   
    
    void OnEnable()
    {
        if (distortionMapShader == null)
            distortionMapShader = Shader.Find("Diem/LensDistortionUVMap");

        if (distortionMapShader == null)
        {
            Debug.LogError("Could not find LensDistortionUVMap shader. Lens distortion will be disabled.");
            enabled = false;
            return;
        }

        distortionMaterial = new Material(distortionMapShader);
    }


    void OnRenderImage(RenderTexture source, RenderTexture destination)
    {
        //Graphics.Blit(distortionMap, destination, distortionMaterial);      // Blit to default output

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
