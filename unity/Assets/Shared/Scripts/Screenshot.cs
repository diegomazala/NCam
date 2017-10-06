using UnityEngine;
using System.Collections;
using System.IO;

public class Screenshot : MonoBehaviour 
{
    public Camera cam;
    private bool recording = false;
    private string recordFolder;

    public UnityEngine.UI.Text recordToggleText;

    void Start()
    {
        if (cam == null)
        {
            enabled = false;
            return;
        }

    }


    public void OnRecordToggleChange(bool toggle)
    {
        if (toggle)
        {
            recordToggleText.text = "Stop Recording";
            this.StartRecord();
        }
        else
        {
            recordToggleText.text = "Start Recording";
            this.StopRecord();
        }
    }


    public void OnTakeScreenshotButtonClick()
    {
        this.TakeScreenshot();
    }


    void Update()
    {
        if (recording)
        {
            ScreenCapture.CaptureScreenshot(recordFolder + Time.time.ToString() + ".png");
        }

        if (Input.GetKeyDown(KeyCode.S))
        {
            System.DateTime t = System.DateTime.Now;
            ScreenCapture.CaptureScreenshot(Application.dataPath + "/../Screenshot_" + t.Hour.ToString("00") + "-" + t.Minute.ToString("00") + "-" + t.Second.ToString("00") + ".png");
        }
    }


    void StartRecord()
    {
        recording = true;
        System.DateTime t = System.DateTime.Now;
        recordFolder = Application.dataPath + "/../Screenshots_" + t.Hour.ToString("00") + "-" + t.Minute.ToString("00") + "/";
        System.IO.DirectoryInfo dir = new System.IO.DirectoryInfo(recordFolder);
        if (!dir.Exists)
            dir.Create();
    }


    void StopRecord()
    {
        recording = false;
    }


    void TakeScreenshot()
    {
        System.DateTime t = System.DateTime.Now;
        ScreenCapture.CaptureScreenshot(Application.dataPath + "/../Screenshot_" + t.Hour.ToString("00") + '-' + t.Minute.ToString("00") + '-' + t.Second.ToString("00") + ".png");
    }

}
