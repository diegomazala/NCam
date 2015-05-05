using UnityEngine;
using System.Collections;



public class LensGUI : MonoBehaviour
{
    public string lensFileName = @"LensTable.xml";
    public LensTable lensTable = null;
    public LensSample testLens = null;

    public string lensPath;

#region GUI Variables
    public Texture2D textureGUI;
    public bool showGUI = false;
    private KeyCode guiTrigger = KeyCode.F2;
    private GUIWindow windowGUI = null;
    private Vector2 scrollPosition;
#endregion

    private int[] lZoom;
    private int[] lFocus;
    private double[] lFov;


    static public string xmlConfigFolder
    {
        get
        {
#if UNITY_EDITOR
            System.IO.DirectoryInfo mainFolderPath = new System.IO.DirectoryInfo(Application.dataPath + "/");
#else
            System.IO.DirectoryInfo mainFolderPath = new System.IO.DirectoryInfo(Application.dataPath + "/../");
#endif
            return mainFolderPath.FullName + @"Config/Lens/";
        }
    }

    void Start()
    {
        //Setup();
    }

    public void Setup()
    {        
        lensTable = new LensTable();
        testLens = new LensSample();

        lensPath = xmlConfigFolder + lensFileName;
        lensTable.Reset();
        if (!lensTable.Load(lensPath))
        {
            Debug.LogError("Could not load lens table : '" + lensPath + "'");
        }

        BuildTextureLens(256, 90);
    }


    void Update()
    {
        if (Input.GetKeyDown(guiTrigger) && Input.GetKey(KeyCode.LeftControl))
            showGUI = !showGUI;
    }


    void OnGUI()
    {
        if (!showGUI || lensTable == null)
            return;

        ShowGUI();
    }

    public void ShowGUI()
    {
        if (windowGUI == null)
        {
            int win_w = 600;
            int win_h = 480;
            windowGUI = new GUIWindow(Screen.width - 20 - win_w, 10, win_h, win_h, "Lens Table");
        }

        windowGUI.rect = GUILayout.Window(windowGUI.id, windowGUI.rect, BuildWindow, windowGUI.label);
    }


    void BuildWindow(int windowID)
    {
        scrollPosition = GUILayout.BeginScrollView(scrollPosition, "box", GUILayout.Width(600), GUILayout.Height(480));
        {
            GUILayout.Space(15);

            // Lens Info GUI
            GUILayout.BeginVertical("box");
            {
                GUILayout.BeginHorizontal();
                {
                    GUILayout.Label("Manufacturer", GUILayout.MaxWidth(80));
                    lensTable.info.manufacturer = GUILayout.TextField(lensTable.info.manufacturer, "box");
                    GUILayout.Space(20);
                    GUILayout.Label("Patrimony Code", GUILayout.MaxWidth(100));
                    lensTable.info.patrimonyCode = GUILayout.TextField(lensTable.info.patrimonyCode, "box");
                }
                GUILayout.EndHorizontal();

                GUILayout.BeginHorizontal();
                {
                    GUILayout.Label("Model", GUILayout.MaxWidth(50));
                    lensTable.info.model = GUILayout.TextField(lensTable.info.model, "box");
                    lensTable.info.range = GUILayout.TextField(lensTable.info.range, "box");
                }
                GUILayout.EndHorizontal();
            }
            GUILayout.EndVertical();

            GUILayout.Space(5);

            // Up Panel = Focus Samples
            GUILayout.BeginHorizontal("box");
            {
                GUILayout.Label("Z | F", "box", GUILayout.MinWidth(50));

                for (int i = 0; i < lensTable.focusSamples.Length; ++i)
                {
                    lensTable.focusSamples[i] = GUIAux.Double(lensTable.focusSamples[i], "{0:0.000}", "box", GUILayout.MinWidth(30), GUILayout.MinHeight(24));
                }
            }
            GUILayout.EndHorizontal();


            GUILayout.BeginHorizontal();
            {
                // Left Panel = Zoom Samples
                GUILayout.BeginVertical("box", GUILayout.MaxWidth(85));
                {
                    for (int i = 0; i < lensTable.zoomSamples.Length; ++i )
                    {
                        lensTable.zoomSamples[i] = GUIAux.Double(lensTable.zoomSamples[i], "{0:0.000}", "box", GUILayout.MinWidth(30), GUILayout.MinHeight(24));
                    }
                }
                GUILayout.EndVertical();

                if (lensTable.lensSamples.Count > 0)
                {
                    // Central Panel = Fov Samples
                    for (int f = 0; f < lensTable.focusSamples.Length; ++f)
                    {
                        GUILayout.BeginVertical();
                        {
                            for (int z = 0; z < lensTable.zoomSamples.Length; ++z)
                            {
                                LensSample l = (LensSample)lensTable.lensSamples[z * lensTable.focusSamples.Length + f];
                                l.fov = GUIAux.Double(l.fov, "{0:0.000}", "box", GUILayout.MinWidth(30), GUILayout.MinHeight(24));
                            }
                        }
                        GUILayout.EndVertical();
                    }
                }
            }
            GUILayout.EndHorizontal();

            GUILayout.Space(5);


            GUILayout.BeginHorizontal();
            {

                GUILayout.BeginVertical();
                {

                    GUILayout.BeginHorizontal("box");
                    {
                        if (GUILayout.Button("Rebuild"))
                        {
                            lensTable.BuildLensTable();
                            BuildTextureLens(textureGUI.width, textureGUI.height);
                        }

                        if (GUILayout.Button("Load"))
                        {
                            this.Load(lensPath);
                        }
                        if (GUILayout.Button("Save"))
                        {
                            this.Save(lensPath);
                        }
                    }
                    GUILayout.EndHorizontal();

                    GUILayout.BeginHorizontal("box");
                    {

                        testLens.zoom = GUIAux.Double(testLens.zoom, "box", GUILayout.MinWidth(40));
                        testLens.focus = GUIAux.Double(testLens.focus, "box", GUILayout.MinWidth(40));
                        testLens.fov = GUIAux.Double(testLens.fov, "box", GUILayout.MinWidth(40));

                        if (GUILayout.Button("Calculate"))
                        {
                            testLens.fov = lensTable.Fov(testLens.zoom, testLens.focus);
                        }

                    }
                    GUILayout.EndHorizontal();

                }
                GUILayout.EndVertical();

                GUILayout.Space(10);
                GUILayout.Label(textureGUI);

            }
            GUILayout.EndHorizontal();

        }
        GUILayout.EndScrollView();

        GUI.DragWindow();
    }



    public void Save(string path)
    {        
        lensTable.Save(lensPath, lensTable.lensSamples);
    }


    void Load(string path)
    {
        Setup();
        lensTable.Reset();
        if (!lensTable.Load(lensPath))
        {
            Debug.LogError("Could not load lens table : '" + lensPath + "'");
        }
    }


    void BuildTextureLens(int tw, int th)
    {
        if (textureGUI == null || textureGUI.width != tw || textureGUI.height != th)
        {
            textureGUI = new Texture2D(tw, th);
        }

        for (int y = 0; y < th; ++y)
        {
            for (int x = 0; x < tw; ++x)
            {
                float f = lensTable.Fov(x * lensTable.zoomMaxValue / (double)tw, y * lensTable.focusMaxValue / (double)th);
                Color color = new Color(x / (float)tw, y / (float)th, f);
                textureGUI.SetPixel(x, y, color);
            }
        }
        textureGUI.Apply();
    }


}
