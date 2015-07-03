using UnityEngine;
using System.Collections;

public class LensControl : MonoBehaviour 
{
    public LensTable lensTable;
    public LensEncoder lensEncoder;


	void OnEnable() 
    {
        if (lensTable == null)
            lensTable = FindObjectOfType<LensTable>();


        if (lensTable == null)
        {
            Debug.LogError("Could not find LensTable object. LensControl will be disabled.");
            enabled = false;
            return;
        }


        if (lensEncoder == null)
            lensEncoder = FindObjectOfType<LensEncoder>();


        if (lensEncoder == null)
        {
            Debug.LogError("Could not find LensEncoder object. LensControl will be disabled.");
            enabled = false;
            return;
        }
	}
	


	void Update () 
    {
        lensTable.UpdateCameraLens(lensEncoder.ZoomNormalized, lensEncoder.FocusNormalized);
	}
}
