using UnityEngine;
using System.Collections;

public class LensEncoderUI : MonoBehaviour
{
    public LensEncoder lensEncoder;

    public UnityEngine.UI.Button connectButton;
    public UnityEngine.UI.Button disconnectButton;
    public UnityEngine.UI.Text portText;
    public UnityEngine.UI.Text statusText;
    public UnityEngine.UI.Text zoomNormalizedText;
    public UnityEngine.UI.Text zoomMappedText;
    public UnityEngine.UI.Text focusNormalizedText;
    public UnityEngine.UI.Text focusMappedText;
    public UnityEngine.UI.Text irisNormalizedText;
    public UnityEngine.UI.Text irisMappedText;


	void OnEnable () 
    {
        if (lensEncoder == null)
        {
            lensEncoder = FindObjectOfType<LensEncoder>();
            if (lensEncoder == null)
            {
                Debug.LogError(this.GetType().FullName + "." + System.Reflection.MethodBase.GetCurrentMethod().Name
                    + " : Could not find LensEncoder component. Missing reference.");
                enabled = false;
                return;
            }
        }

        UpdateUI();
	}
	
    void Update()
    {
        UpdateUI();
    }

    public void OnConnect()
    {
        lensEncoder.Connect();
        UpdateUI();
    }

    public void OnDisconnect()
    {
        lensEncoder.Disconnect();
        UpdateUI();
                
    }

    public void UpdateUI()
    {
        connectButton.interactable = !lensEncoder.IsConnected();
        disconnectButton.interactable = lensEncoder.IsConnected();
        portText.text = lensEncoder.port.ToString();
        statusText.text = lensEncoder.IsConnected() ? "Connected" : "Disconnected";
        
        zoomNormalizedText.text = lensEncoder.ZoomNormalized.ToString();
        focusNormalizedText.text = lensEncoder.FocusNormalized.ToString();
        irisNormalizedText.text = lensEncoder.IrisNormalized.ToString();

        zoomMappedText.text = lensEncoder.ZoomMapped.ToString();
        focusMappedText.text = lensEncoder.FocusMapped.ToString();
        irisMappedText.text = lensEncoder.IrisMapped.ToString();
    }
}
