
using UnityEngine;
using System.Collections;

public class DrawGrid3D : MonoBehaviour 
{
    public Material mat;
    public Vector2 cellSize;
    public Vector2 resolution;
    public Vector3 gridPosition;
    public bool showGrid = true;
    public bool doubleLines = true;
    public bool verticalGrid = true;




    void OnPostRender() 
    {
        if (!showGrid)
            return;

        if (!mat) 
        {
            Debug.LogError("Please Assign a material on the inspector");
            return;
        }

        GL.PushMatrix();
        {
            mat.SetPass(0);
            
            GL.Begin(GL.LINES);
            {
                //
                // VERTICAL GRID
                //
                if (verticalGrid)
                {
                    Vector3 gridpos = new Vector3(gridPosition.x - resolution.x * cellSize.x * 0.5f, gridPosition.x - resolution.y * cellSize.y * 0.5f, gridPosition.z);
                    float shift = cellSize.magnitude / resolution.magnitude;

                    // drawing vertical lines
                    for (int x = 0; x <= resolution.x; ++x)
                    {
                        GL.Vertex3(gridpos.x + x * cellSize.x, gridpos.y, gridpos.z);
                        GL.Vertex3(gridpos.x + x * cellSize.x, gridpos.y + resolution.y * cellSize.y, gridpos.z);
                    }

                    // drawing horizontal lines
                    for (int y = 0; y <= resolution.y; ++y)
                    {
                        GL.Vertex3(gridpos.x, gridpos.y + y * cellSize.y, gridpos.z);
                        GL.Vertex3(gridpos.x + resolution.x * cellSize.x, gridpos.y + y * cellSize.y, gridpos.z);
                    }


                    //
                    // SHIFT FOR LINE WIDTH
                    //
                    if (doubleLines)
                    {
                        // drawing vertical lines
                        for (int x = 0; x <= resolution.x; ++x)
                        {
                            GL.Vertex3(shift + gridPosition.x + x * cellSize.x, gridPosition.y, gridPosition.z);
                            GL.Vertex3(shift + gridPosition.x + x * cellSize.x, gridPosition.y + resolution.y * cellSize.y, gridPosition.z);
                        }

                        // drawing horizontal lines
                        for (int y = 0; y <= resolution.y; ++y)
                        {
                            GL.Vertex3(gridPosition.x, shift + gridPosition.y + y * cellSize.y, gridPosition.z);
                            GL.Vertex3(gridPosition.x + resolution.x * cellSize.x, shift + gridPosition.y + y * cellSize.y, gridPosition.z);
                        }
                    }
                }

            }
            GL.End();
        }
        GL.PopMatrix();
    }

    void Update()
    {
        if (Input.GetKeyDown(KeyCode.G) && Input.GetKey(KeyCode.LeftControl))
            showGrid = !showGrid;
    }
}
