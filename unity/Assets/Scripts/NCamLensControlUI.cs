using UnityEngine;
using System.Collections;

public class NCamLensControlUI : MonoBehaviour 
{
    public LensTable lensTable = null;
    public NCam ncam = null;

    public UnityEngine.UI.Text fovText;

    private Camera targetCam = null;
    private NCamEncoder ncamEncoder = null;

    private bool lensTableToggled = false;

    void Start()
    {
        if (lensTable == null)
            lensTable = FindObjectOfType<LensTable>();

        if (lensTable == null)
        {
            Debug.LogError("Could not find LensTable object. NCamLensControlUI will be disabled.");
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


        NCamLensEncoderUI ncamEncoderUI = FindObjectOfType<NCamLensEncoderUI>();
        if (ncamEncoderUI != null)
            ncamEncoder = ncamEncoderUI.ncamEncoder;

        targetCam = ncam.targetCamera[0];

        OnLensTableToggle(false);
    }


    public void OnNCamToggle(bool toggle)
    {
        if (ncam != null)
            ncam.Distortion = toggle;
    }


    public void OnLensTableToggle(bool toggle)
    {
        lensTableToggled = toggle;
        if (lensTable != null)
        {
            lensTable.Distortion = toggle;
        }
    }

    // On late update, the camera values will be owerwritten by LensTable
    void LateUpdate()
    {
        if (lensTableToggled && ncamEncoder != null)
        {
            float zoom = (float)ncamEncoder.GetValue(NCamEncoder.EParameter.ZoomNormalized);
            float focus = (float)ncamEncoder.GetValue(NCamEncoder.EParameter.FocusNormalized);
            lensTable.zoom = zoom;
            lensTable.focus = focus;
            lensTable.UpdateCameraLens(zoom, focus);
        }

        UpdateUI();
    }

    public void UpdateUI()
    {
        fovText.text = targetCam.fieldOfView.ToString();
    }

}
