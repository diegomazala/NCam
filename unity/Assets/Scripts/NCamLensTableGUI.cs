using UnityEngine;
using System.Collections;


[RequireComponent(typeof(NCamLens))]
public class NCamLensTableGUI : MonoBehaviour
{
    public NCamLens ncamLens = null;
    public string lensFileName = @"LensTable.xml";
    private LensTable lensTable = null;
    private LensSample testLens = new LensSample();

    private LensInfo lensInfo = null;

    private string lensPath;

#region GUI Variables
    public Texture2D textureGUI;
    public bool showGUI = false;
    private KeyCode guiTrigger = KeyCode.F2;
    private GUIWindow windowGUI = null;
    private Vector2 scrollPosition;
    private bool createNewTablePressed = false;
    private int guiTexWidth = 256;
    private int guiTexHeight = 90;
#endregion

    private int[] lZoom;
    private int[] lFocus;
    private double[] lFov;

    public bool fovFromEncoder = true;
    
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
        if (ncamLens == null)
            ncamLens = GetComponent<NCamLens>();
    }
    
    void Setup()
    {        
        lensTable = new LensTable();
        

        lensPath = xmlConfigFolder + lensFileName;
        //lensTable.Reset();
        if (!lensTable.Load(lensPath))
        {
            Debug.LogError("Could not load lens table : '" + lensPath + "'");
        }

        BuildTextureLens(guiTexWidth, guiTexHeight);
    }

    void CreateNewLens()
    {
        lensTable = new LensTable();

        if (lensInfo != null)
            lensTable.info = lensInfo;
        
        lensTable.Reset();


        // create empty texture
        textureGUI = new Texture2D(guiTexWidth, guiTexHeight);
    }

    void DestroyLens()
    {
        lensTable = null;
        Destroy(textureGUI);
    }

    void Update()
    {
        if (Input.GetKeyDown(guiTrigger) && Input.GetKey(KeyCode.LeftControl))
            showGUI = !showGUI;
    }


    void OnGUI()
    {
        if (!showGUI)
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
            GUILayout.BeginHorizontal("box");
            {
                if (!createNewTablePressed)
                {
                    if (GUILayout.Button("New"))
                    {
                        createNewTablePressed = true;
                        this.DestroyLens();
                        lensInfo = new LensInfo();
                    }
                }

                if (createNewTablePressed)
                {
                    GUILayout.Label("Samples Count:", "box");
                    GUILayout.Space(15);
                    GUILayout.BeginHorizontal();
                    {
                        GUILayout.Label("Zoom", "box");
                        lensInfo.zoomSamples = GUIAux.Int(lensInfo.zoomSamples, "box");
                    }
                    GUILayout.EndHorizontal();

                    GUILayout.BeginHorizontal();
                    {
                        GUILayout.Label("Focus", "box");
                        lensInfo.focusSamples = GUIAux.Int(lensInfo.focusSamples, "box");
                    }
                    GUILayout.EndHorizontal();
                                        
                }
                else
                {
                    if (GUILayout.Button("Rebuild"))
                    {
                        lensTable.BuildLensTable();
                        BuildTextureLens(guiTexWidth, guiTexHeight);
                    }

                    if (GUILayout.Button("Load"))
                    {
                        this.Load(lensPath);
                    }

                    if (GUILayout.Button("Save"))
                    {
                        this.Save(@xmlConfigFolder + lensInfo.manufacturer + '_' + lensInfo.model + ".xml");
                    }
                }
                
            }
            GUILayout.EndHorizontal();


            if (lensInfo != null)
                this.LensInfoGUI(lensInfo);


            if (createNewTablePressed)
            {
                GUILayout.BeginHorizontal();
                {
                    if (GUILayout.Button("Cancel"))
                    {
                        if (createNewTablePressed)
                            createNewTablePressed = false;
                    }
                    if (GUILayout.Button("Create"))
                    {
                        if (createNewTablePressed)
                        {
                            createNewTablePressed = false;
                            this.CreateNewLens();
                        }
                    }
                }
                GUILayout.EndHorizontal();
            }

            GUILayout.Space(5);

            // is there lens loaded?
            if (lensTable == null)
            {
                GUILayout.Label("No Lens", "box", GUILayout.MinWidth(50));
            }
            else
            {

                // Up Panel = Focus Samples
                GUILayout.BeginHorizontal("box");
                {
                    if (GUILayout.Button("Z | F", "box", GUILayout.MinWidth(50)))
                    {
                        fovFromEncoder = !fovFromEncoder;
                    }

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

                                    if (!fovFromEncoder)
                                    {
                                        l.fov = GUIAux.Double(l.fov, "{0:0.000}", "box", GUILayout.MinWidth(30), GUILayout.MinHeight(24));
                                    }
                                    else
                                    {
                                        if (GUILayout.Button(l.fov.ToString("0.000"), GUILayout.MinWidth(30), GUILayout.MinHeight(24)))
                                        {
                                            l.fov = NCamPlugin.NCamFovVertical();
                                            l.zoom = lensTable.zoomSamples[z];
                                            l.focus = lensTable.focusSamples[f];
                                        }
                                    }
                                    
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

            } // if-else : is there lens loaded?
        }
        GUILayout.EndScrollView();

        GUI.DragWindow();
    }

    void LensInfoGUI(LensInfo info)
    {
        // Lens Info GUI
        GUILayout.BeginVertical("box");
        {
            GUILayout.BeginHorizontal();
            {
                GUILayout.Label("Manufacturer", GUILayout.MaxWidth(80));
                info.manufacturer = GUILayout.TextField(info.manufacturer, "box");
                GUILayout.Space(20);
                GUILayout.Label("Patrimony Code", GUILayout.MaxWidth(100));
                info.patrimonyCode = GUILayout.TextField(info.patrimonyCode, "box");
            }
            GUILayout.EndHorizontal();

            GUILayout.BeginHorizontal();
            {
                GUILayout.Label("Model", GUILayout.MaxWidth(50));
                info.model = GUILayout.TextField(info.model, "box");
                info.range = GUILayout.TextField(info.range, "box");
            }
            GUILayout.EndHorizontal();
        }
        GUILayout.EndVertical();
    }

    public void Save(string path)
    {
        lensTable.Save(path, lensTable.lensSamples);
    }


    void Load(string path)
    {
        lensPath = xmlConfigFolder + lensFileName;
        lensTable = new LensTable();

        if (!lensTable.Load(lensPath))
        {
            Debug.LogError("Could not load lens table : '" + lensPath + "'");
        }

        BuildTextureLens(guiTexWidth, guiTexHeight);
    }


    void BuildTextureLens(int tw, int th)
    {
        if (textureGUI == null)
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
