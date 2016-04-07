using UnityEngine;
using System.Collections;

[ExecuteInEditMode]
[AddComponentMenu("Diem/Screen Grid")]
public class ScreenGrid : ImageEffectBase
{
    public Color color = Color.red;
    public int lineCount = 20;
    public float lineWidth = 0.05f;

    void OnRenderImage(RenderTexture source, RenderTexture destination)
    {
        material.SetColor("_Color", color);
        material.SetFloat("_LineCount", lineCount);
        material.SetFloat("_LineWidth", lineWidth);
        Graphics.Blit(source, destination, material);
    }
}
