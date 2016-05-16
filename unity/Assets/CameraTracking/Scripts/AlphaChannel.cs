using UnityEngine;
using System.Collections;

[ExecuteInEditMode]
[AddComponentMenu("Diem/Alpha Channel")]
public class AlphaChannel : ImageEffectBase
{
    void OnRenderImage(RenderTexture source, RenderTexture destination)
    {
        material.SetTexture("_MainTex", source);
        Graphics.Blit(source, destination, material);
    }
}
