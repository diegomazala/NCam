using UnityEngine;
using System.Collections;

public class NCamScreenInfo : MonoBehaviour 
{
    public NCam ncam = null;
    public UnityEngine.UI.Text screenInfo;

	void Start () 
    {
	    if (ncam == null)
        {
            ncam = FindObjectOfType<NCam>();
        }

        if (ncam == null)
        {
            if (screenInfo != null)
                screenInfo.text = "NCam Missing!";
            
            enabled = false;
        }

	}
	
	void Update () 
    {
	    if (screenInfo != null)
        {
            screenInfo.text = "Frame Lost: " + NCamPlugin.NCamFrameLostCount().ToString() + " \nField Lost: " + NCamPlugin.NCamFieldLostCount();
        }
	}
}
