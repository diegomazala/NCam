using UnityEngine;
using System.Collections;

public class LensEncoderUI : MonoBehaviour
{
    public LensEncoder lensEncoder;

    public UnityEngine.UI.Button connectButton;
    public UnityEngine.UI.Button disconnectButton;
    public UnityEngine.UI.Text portText;
    public UnityEngine.UI.Text statusText;
    public UnityEngine.UI.Text zoomText;
    public UnityEngine.UI.Text focusText;
    public UnityEngine.UI.Text irisText;


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
        //Debug.Log(this.GetType().FullName + "." + System.Reflection.MethodBase.GetCurrentMethod().Name);

        lensEncoder.Connect();
        UpdateUI();
    }

    public void OnDisconnect()
    {
        //Debug.Log(this.GetType().FullName + "." + System.Reflection.MethodBase.GetCurrentMethod().Name);
        lensEncoder.Disconnect();
        UpdateUI();
    }

    public void UpdateUI()
    {
        connectButton.interactable = !lensEncoder.IsConnected();
        disconnectButton.interactable = lensEncoder.IsConnected();
        
        statusText.text = lensEncoder.IsConnected() ? "Connected" : "Disconnected";
        //statusText.text += "\t(" + xcito.GetErrorStatus() + ')';

        portText.text = lensEncoder.port.ToString();

        zoomText.text = lensEncoder.Zoom.ToString();
        focusText.text = lensEncoder.Focus.ToString();
        irisText.text = lensEncoder.Iris.ToString();
    }
}
