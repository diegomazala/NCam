
using UnityEngine;
using System.Collections;


[ExecuteInEditMode]
public class DrawGrid2D : MonoBehaviour 
{
    public Material mat;
    public Vector2 resolution = new Vector2(50.0f, 50.0f);

    public bool showGrid = false;

    void Start()
    {
        if (mat == null)
        {
            mat = new Material(Shader.Find("VertexLit"));
            mat.color = Color.red;
        }

    }

    void Update()
    {
        if (Input.GetKeyDown(KeyCode.G) && Input.GetKeyDown(KeyCode.LeftControl))
            showGrid = !showGrid;
    }

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
            GL.LoadOrtho();
            GL.Begin(GL.LINES);
            {

                float width = 1.0f / resolution.x;
                float height = 1.0f / resolution.y;

                for (int x = 0; x < resolution.x; ++x)
                {
                    GL.Vertex3(x * width, 0, 0);
                    GL.Vertex3(x * width, resolution.y, 0);
                }

                for (int y = 0; y < resolution.y; ++y)
                {
                    GL.Vertex3( 0, y * height, 0);
                    GL.Vertex3( resolution.x, y * height, 0);
                }
            }
            GL.End();
        }
        GL.PopMatrix();
    }

}
