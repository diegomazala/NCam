using UnityEngine;
using System.Collections;

public class NCamLensEncoderUI : MonoBehaviour
{
    public NCam ncam;
    public NCamEncoder ncamEncoder;

    public UnityEngine.UI.Text zoomNormalizedText;
    public UnityEngine.UI.Text zoomMappedText;
    public UnityEngine.UI.Text focusNormalizedText;
    public UnityEngine.UI.Text focusMappedText;
    public UnityEngine.UI.Text irisNormalizedText;
    public UnityEngine.UI.Text irisMappedText;


	void OnEnable () 
    {
        if (ncam == null)
        {
            ncam = FindObjectOfType<NCam>();
            if (ncam == null)
            {
                Debug.LogError(this.GetType().FullName + "." + System.Reflection.MethodBase.GetCurrentMethod().Name
                    + " : Could not find NCam component. Missing reference.");
                enabled = false;
                return;
            }
        }


        ncamEncoder = new NCamEncoder();

        UpdateUI();
	}
	
    void Update()
    {
        NCamPlugin.NCamEncoderParameters(ncamEncoder.Ptr());
        UpdateUI();
    }
    

    public void UpdateUI()
    {
        zoomNormalizedText.text = ncamEncoder.GetValue(NCamEncoder.EParameter.ZoomNormalized).ToString();
        focusNormalizedText.text = ncamEncoder.GetValue(NCamEncoder.EParameter.FocusNormalized).ToString();
        irisNormalizedText.text = ncamEncoder.GetValue(NCamEncoder.EParameter.IrisNormalized).ToString();

        zoomMappedText.text = ncamEncoder.GetValue(NCamEncoder.EParameter.ZoomMapped).ToString();
        focusMappedText.text = ncamEncoder.GetValue(NCamEncoder.EParameter.FocusMapped).ToString();
        irisMappedText.text = ncamEncoder.GetValue(NCamEncoder.EParameter.IrisMapped).ToString();
    }
}
