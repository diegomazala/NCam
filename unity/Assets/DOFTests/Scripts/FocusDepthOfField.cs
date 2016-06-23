using UnityEngine;
using System.Collections;
using System.Collections.Generic;



[System.Serializable]
public class FocusMap
{
    public string LensManufacturer { get; set; }
    public string LensModel { get; set; }

    public AnimationCurve Map = null;

    public FocusMap()
    {
        Map = new AnimationCurve();
    }

    public float FocalDistance(float encoder_value)
    {
        return Map.Evaluate(encoder_value);
    }

    public float MinFocalDistance
    {
        get { return Map.Evaluate(0.0f); }
    }

    public float MaxFocalDistance
    {
        get { return Map.Evaluate(1.0f); }
    }
}

[System.Serializable]
public class FujinonLensTest : FocusMap
{
    public FujinonLensTest()
    {
        Map.AddKey(0.00f, 0.8f);
        Map.AddKey(0.10f, 0.9f);
        Map.AddKey(0.20f, 1.0f);
        Map.AddKey(0.33f, 1.2f);
        Map.AddKey(0.47f, 1.5f);
        Map.AddKey(0.60f, 2.0f);
        Map.AddKey(0.73f, 3.0f);
        Map.AddKey(0.83f, 5.0f);
        Map.AddKey(0.90f, 10.0f);
        Map.AddKey(1.00f, 20.0f);
    }
}



public class FocusDepthOfField : MonoBehaviour 
{
    public FocusMap focusLens = new FujinonLensTest();

    public UnityStandardAssets.CinematicEffects.DepthOfField[] dof;

    public LensEncoder encoder = null;

    public float result = 0.0f;
    public float plane = 0.0f;
    public Transform target = null;

    void OnEnable()
    {
        if (encoder == null)
            encoder = FindObjectOfType<LensEncoder>();

        if (encoder == null)
            enabled = false;

        if (target == null)
            enabled = false;
        //else
        //{
        //    foreach (UnityStandardAssets.CinematicEffects.DepthOfField d in dof)
        //        d.focus.transform = target;
        //}
    }
	
	void Update () 
    {
        result = focusLens.FocalDistance(encoder.FocusNormalized);

#if false
        //foreach (UnityStandardAssets.CinematicEffects.DepthOfField d in dof)
        {
            Camera cam = dof[0].GetComponent<Camera>();
            Vector3 v = cam.transform.forward * result;
            //cam.forward * result;
            target.position = v;
        }
#else

        dof[0].focus.plane = result / focusLens.MaxFocalDistance;
        plane = dof[0].focus.plane;
#endif



        
	}
}
