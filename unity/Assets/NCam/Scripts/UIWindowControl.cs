using UnityEngine;
using System.Collections;

public class UIWindowControl : MonoBehaviour 
{

    public GameObject[] uiObjects;
    protected UnityEngine.UI.Image imagePanel = null;

    void Start()
    {
        imagePanel = GetComponent<UnityEngine.UI.Image>();
    }

    public void OnEnableUI(bool toogle)
    {
        foreach (GameObject go in uiObjects)
            go.SetActive(toogle);

        if (imagePanel)
            imagePanel.enabled = toogle;
    }


}
