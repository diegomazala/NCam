using UnityEngine;
using System.Collections;

public class ScreenGridUI : MonoBehaviour
{
    public ScreenGrid[] grid;


    void Start()
    {
        if (grid.Length < 0)
            enabled = false;
    }



    public void OnShowGridToggleChange(bool toggle)
    {
        foreach (ScreenGrid g in grid)
            g.enabled = toggle;
    }


    public void OnLineCountSliderChange(System.Single line_count)
    {
        foreach (ScreenGrid g in grid)
            g.lineCount = (int)line_count;
    }

    public void OnLineWidthSliderChange(System.Single line_width)
    {
        foreach (ScreenGrid g in grid)
            g.lineWidth = line_width;
    }



}
