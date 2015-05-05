using UnityEngine;
using System.Collections;

public class DrawGrid3DGUI : MonoBehaviour 
{
    public DrawGrid3D[] grid;
    public UnityEngine.UI.Slider cellSizeXSlider;
    public UnityEngine.UI.Slider cellSizeYSlider;
    public UnityEngine.UI.Text cellSizeScaleLabel;

    private float scale = 1.0f;

    void OnEnable()
    {
        if (grid.Length < 0)
            enabled = false;

        cellSizeXSlider.wholeNumbers = false;
        cellSizeYSlider.wholeNumbers = false;

        cellSizeScaleLabel.text = "1 - 100 cm";

        cellSizeXSlider.minValue = 1.0f;
        cellSizeXSlider.maxValue = 100.0f;

        cellSizeYSlider.minValue = 1.0f;
        cellSizeYSlider.maxValue = 100.0f;

        cellSizeXSlider.value = 50.0f;
        cellSizeYSlider.value = 50.0f;

        foreach (DrawGrid3D dg in grid)
            dg.cellSize = new Vector2(cellSizeXSlider.value, cellSizeYSlider.value);

        foreach (DrawGrid3D dg in grid)
            dg.resolution = new Vector2(100, 100);
    }
	


    public void OnShowGridToggleChange(bool toggle)
    {
        foreach (DrawGrid3D dg in grid)
            dg.showGrid = toggle;
    }


    public void OnResolutionXSliderChange(System.Single res_x)
    {
        foreach (DrawGrid3D dg in grid)
            dg.resolution = new Vector2(res_x, dg.resolution.y);
    }


    public void OnResolutionYSliderChange(System.Single res_y)
    {
        foreach (DrawGrid3D dg in grid)
            dg.resolution = new Vector2(dg.resolution.x, res_y);
    }


    public void OnCellSizeXSliderChange(System.Single res_x)
    {
        foreach (DrawGrid3D dg in grid)
            dg.cellSize = new Vector2(res_x * scale, dg.cellSize.y);
    }


    public void OnCellSizeYSliderChange(System.Single res_y)
    {
        foreach (DrawGrid3D dg in grid)
            dg.cellSize = new Vector2(dg.cellSize.x, res_y * scale);
    }


    public void OnCellSizeScaleToggleChange(bool toggle)
    {
        if (toggle)
        {
            cellSizeScaleLabel.text = "0.1 - 1 m";
            
            cellSizeXSlider.minValue = 0.5f;
            cellSizeXSlider.maxValue = 1.0f;

            cellSizeYSlider.minValue = 0.5f;
            cellSizeYSlider.maxValue = 1.0f;

            cellSizeXSlider.value = 0.5f;
            cellSizeYSlider.value = 0.5f;
        }
        else
        {
            cellSizeScaleLabel.text = "1 - 100 cm";

            cellSizeXSlider.minValue = 1.0f;
            cellSizeXSlider.maxValue = 100.0f;

            cellSizeYSlider.minValue = 1.0f;
            cellSizeYSlider.maxValue = 100.0f;

            cellSizeXSlider.value = 50f;
            cellSizeYSlider.value = 50f;
        }
    }
	
}
