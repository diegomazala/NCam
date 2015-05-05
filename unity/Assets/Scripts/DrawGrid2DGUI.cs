using UnityEngine;
using System.Collections;

public class DrawGrid2DGUI : MonoBehaviour 
{
    public DrawGrid2D[] grid;


    void Start()
    {
        if (grid.Length < 0)
            enabled = false;
    }
	


    public void OnShowGridToggleChange(bool toggle)
    {
        foreach (DrawGrid2D dg in grid)
            dg.showGrid = toggle;
    }


    public void OnResolutionXSliderChange(System.Single res_x)
    {
        foreach (DrawGrid2D dg in grid)
            dg.resolution = new Vector2(res_x, dg.resolution.y);
    }


    public void OnResolutionYSliderChange(System.Single res_y)
    {
        foreach (DrawGrid2D dg in grid)
            dg.resolution = new Vector2(dg.resolution.x, res_y);
    }
	

	
}
