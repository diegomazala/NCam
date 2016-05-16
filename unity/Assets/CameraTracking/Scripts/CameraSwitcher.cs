using UnityEngine;
using System.Collections;

[System.Serializable]
public class CameraTrigger
{
    public Camera Camera;
    public KeyCode Key;
}


public class CameraSwitcher : MonoBehaviour 
{
    public CameraTrigger[] m_KeyCam;
    public int m_CurrentCamera = 0;

    public BlitTargetTexture targetCam = null;

	void Start () 
    {
        if (m_KeyCam.Length < 1)
            this.enabled = false;


        for (int i = 0; i < m_KeyCam.Length; ++i)
        {
            m_KeyCam[i].Camera.depth = 0;
        }

        if (m_KeyCam.Length > m_CurrentCamera)
		{
            m_KeyCam[m_CurrentCamera].Camera.depth = 1;
            if (targetCam != null && targetCam.m_TargetCamera != null)
                targetCam.m_TargetCamera = m_KeyCam[m_CurrentCamera].Camera;
		}

        
	}
	

	void Update () 
    {
        for (int i = 0; i < m_KeyCam.Length; ++i)
        {
            m_KeyCam[i].Camera.depth = 0;

            if(Input.GetKeyDown(m_KeyCam[i].Key))
                m_CurrentCamera = i;
        }

        if (m_KeyCam.Length > m_CurrentCamera)
        {
            m_KeyCam[m_CurrentCamera].Camera.depth = 1;
            if (targetCam != null && targetCam.m_TargetCamera != null)
                targetCam.m_TargetCamera = m_KeyCam[m_CurrentCamera].Camera;
        }
	}
}
