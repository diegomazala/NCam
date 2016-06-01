using UnityEngine;
using System.Collections;

public class NCamUI : MonoBehaviour
{
    public NCam ncam = null;



    public UnityEngine.UI.Button connectButton;
    public UnityEngine.UI.Button disconnectButton;
    public UnityEngine.UI.Toggle autoConnectionToggle;
    public UnityEngine.UI.Image statusImage;
    public UnityEngine.UI.Text statusText;

    public UnityEngine.UI.InputField ipAddress;
    public UnityEngine.UI.InputField port;

    public UnityEngine.UI.Text timeCode;
    public UnityEngine.UI.Image syncFramesImage;
    public UnityEngine.UI.Text syncFramesText;
    public UnityEngine.UI.Image syncFieldsImage;
    public UnityEngine.UI.Text syncFieldsText;
    public UnityEngine.UI.Text droppedFramesText;
    public UnityEngine.UI.Text droppedFieldsText;
    public UnityEngine.UI.Text frameDelayText;
    public UnityEngine.UI.Image fpsImage;
    public UnityEngine.UI.Text fpsText;

    public UnityEngine.UI.Text imageWidthText;
    public UnityEngine.UI.Text imageHeightText;
    public UnityEngine.UI.Text sensorWidthText;
    public UnityEngine.UI.Text sensorHeightText;
    public UnityEngine.UI.Text sensorCenterXText;
    public UnityEngine.UI.Text sensorCenterYText;
    public UnityEngine.UI.Text fovText;
    public UnityEngine.UI.Text aspectText;
    public UnityEngine.UI.Toggle distortionToggle;


    public UnityEngine.UI.Text zoomText;
    public UnityEngine.UI.Text focusText;
    public UnityEngine.UI.Text irisText;

    public UnityEngine.UI.Text positionText;
    public UnityEngine.UI.Text rotationText;


	void OnEnable () 
    {
        if (ncam == null)
        {
            ncam = FindObjectOfType<NCam>();
            if (ncam == null)
            {
                Debug.LogError(this.GetType().FullName + "." + System.Reflection.MethodBase.GetCurrentMethod().Name
                    + " : Could not find NCamConnection component. Missing reference.");
                enabled = false;
                return;
            }
        }
	}


    void Start()
    {
        ipAddress.text = ncam.ipAddress;
        port.text = ncam.port.ToString();
        autoConnectionToggle.isOn = ncam.autoConnection;
        frameDelayText.text = ncam.FrameDelay.ToString();
        distortionToggle.isOn = ncam.Distortion;
    }
	

    public void OnConnectButtonPress()
    {
        ncam.ipAddress = ipAddress.text;
        ncam.port = System.Int32.Parse(port.text);
        ncam.Connect();
    }


    public void OnDisconnectButtonPress()
    {
        ncam.Disconnect();
    }


    public void OnAutoConnectionToggle(bool value)
    {
        ncam.autoConnection = value;
    }


    public void OnDistortionToggle()
    {
        ncam.Distortion = distortionToggle.isOn;
    }

    public void OnUseGLMatricesToggle(bool value)
    {
        ncam.useGLMatrix = value;
    }


    void Update()
    {
        if (!ncam.enabled)
            return;

        UpdateConnectionUI();

        if (ncam.GetStatus() == NCam.Status.Disconnected)
            return;

        UpdateTimingUI();
        UpdateOpticalParamsUI();
        UpdateEncoderParamsUI();
        UpdateTrackingParamsUI();
    }


    void UpdateConnectionUI()
    {
        switch (ncam.GetStatus())
        {
            case NCam.Status.Connected:
                {
                    connectButton.interactable = false;
                    disconnectButton.interactable = true;
                    autoConnectionToggle.interactable = true;
                    statusImage.color = new Color(0.0f, 0.5f, 0.0f);
                    statusText.text = "Connected";
                    break;
                }
            case NCam.Status.Connecting:
                {
                    connectButton.interactable = false;
                    disconnectButton.interactable = false;
                    autoConnectionToggle.interactable = false;
                    statusImage.color = new Color(0.5f, 0.5f, 0.0f);
                    statusText.text = "Connecting";
                    break;
                }
            default:
            case NCam.Status.Disconnected:
                {
                    connectButton.interactable = true;
                    disconnectButton.interactable = false;
                    autoConnectionToggle.interactable = true;
                    statusImage.color = new Color(0.5f, 0.0f, 0.0f);
                    statusText.text = "Disconnected";
                    break;
                }
        }
    }


    void UpdateTimingUI()
    {
        timeCode.text = ncam.Optical.TimeCode.ToString();
        syncFramesText.text = ncam.FrameSync.ToString();
        syncFieldsText.text = ncam.FieldSync.ToString();
        droppedFramesText.text = ncam.DroppedFrames.ToString();
        droppedFieldsText.text = ncam.DroppedFields.ToString();
        frameDelayText.text = ncam.FrameDelay.ToString();
        fpsText.text = ncam.FrameRate.ToString("00.00");

        if (ncam.DroppedFrames > 0)
            syncFramesImage.color = new Color(0.5f, 0.0f, 0.0f);
        else
            syncFramesImage.color = new Color(0.0f, 0.5f, 0.0f);

        if (ncam.DroppedFields > 0)
            syncFieldsImage.color = new Color(0.5f, 0.0f, 0.0f);
        else
            syncFieldsImage.color = new Color(0.0f, 0.5f, 0.0f);

        if (ncam.FrameRate > 59.9)
            fpsImage.color = new Color(0.0f, 0.5f, 0.0f);
        else if (ncam.FrameRate < 59.0)
            fpsImage.color = new Color(0.5f, 0.0f, 0.0f);
        else
            fpsImage.color = new Color(0.5f, 0.5f, 0.0f);
    }


    void UpdateOpticalParamsUI()
    {
        imageWidthText.text = ncam.Optical.ImageResolution.x.ToString();
        imageHeightText.text = ncam.Optical.ImageResolution.y.ToString();
        sensorWidthText.text = ncam.Optical.ImageSensorSize.x.ToString();
        sensorHeightText.text = ncam.Optical.ImageSensorSize.y.ToString();
        sensorCenterXText.text = ncam.Optical.ProjectionCenter.x.ToString();
        sensorCenterYText.text = ncam.Optical.ProjectionCenter.y.ToString();
        fovText.text = ncam.Optical.FieldOfViewVertical.ToString();
        aspectText.text = ncam.Optical.ImageAspectRatio.ToString();
    }


    void UpdateEncoderParamsUI()
    {
        zoomText.text = ncam.Encoder.GetValue(NCamEncoder.EParameter.ZoomNormalized).ToString();
        focusText.text = ncam.Encoder.GetValue(NCamEncoder.EParameter.FocusNormalized).ToString();
        irisText.text = ncam.Encoder.GetValue(NCamEncoder.EParameter.IrisNormalized).ToString();
    }


    void UpdateTrackingParamsUI()
    {
        positionText.text = ncam.Position.ToString();
        rotationText.text = ncam.Rotation.ToString();
    }

     
}
