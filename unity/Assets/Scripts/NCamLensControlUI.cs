using UnityEngine;
using System.Collections;

public class NCamLensControlUI : MonoBehaviour 
{
    public LensControl lensControl = null;
    public NCam ncam = null;

    public UnityEngine.UI.Text fovText;

    private Camera targetCam = null;

    void OnEnable()
    {
        if (lensControl == null)
            lensControl = FindObjectOfType<LensControl>();

        if (lensControl == null)
        {
            Debug.LogError("Could not find LensControl object. NCamLensControlUI will be disabled.");
            enabled = false;
            return;
        }

        if (ncam == null)
            ncam = FindObjectOfType<NCam>();

        if (ncam == null)
        {
            Debug.LogError("Could not find NCam object. NCamLensControlUI will be disabled.");
            enabled = false;
            return;
        }


        targetCam = ncam.targetCamera[0];

       OnNCamToggle(true);
       // OnEncoderToggle(false);
    }


    public void OnNCamToggle(bool toggle)
    {
        if (ncam != null)
        {
            ncam.updateData = toggle;
            ncam.Distortion = toggle;
        }
    }


    public void OnEncoderToggle(bool toggle)
    {
        if (lensControl != null)
        {
            lensControl.lensEncoder.enabled = toggle;
            lensControl.lensTable.enabled = toggle;
            lensControl.lensTable.Distortion = toggle;
            lensControl.enabled = toggle;
        }

    }

    void Update()
    {
        UpdateUI();
    }

    public void UpdateUI()
    {
        fovText.text = targetCam.fieldOfView.ToString();
    }

}
