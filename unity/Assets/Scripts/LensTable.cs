using UnityEngine;
using System.Collections;
using System;
using System.IO;
using System.Xml;
using System.Text.RegularExpressions;


[System.Serializable]
public class LensSample
{
    public double zoom;
    public double focus;
    public double fov;

    public LensSample()
    {
    }

    public LensSample(double z, double f, double _fov)
    {
        zoom = z;
        focus = f;
        fov = _fov;
    }
}



[System.Serializable]
public class LensInfo
{
    public string manufacturer;
    public string model;
    public string range;
    public string patrimonyCode;
    public int zoomSamples;
    public int focusSamples;
    

    public LensInfo()
    {
        manufacturer = "Ex_Fujinon";
        model = "Ex_HA22x7.8BDERD-S48";
        range = "Ex_1:1.8/7.8-172mm";
        patrimonyCode = "Ex_162542";
        zoomSamples = 9;
        focusSamples = 6;
    }
}



[System.Serializable]
public class LensTable 
{
    public LensInfo info = null;
    public ArrayList lensSamples = null;

    public double[] zoomSamples;
    public double[] focusSamples;

    private alglib.spline2d.spline2dinterpolant splineLens;


    public double zoomMaxValue = 0;
    public double focusMaxValue = 0;
    //public bool invertFocusSamplesOrder = false;



	public LensTable() 
    {
        info = new LensInfo();
        splineLens = new alglib.spline2d.spline2dinterpolant();
        lensSamples = new ArrayList();

        ////for (int z = 0; z < info.zoomSamples; ++z)
        ////    for (int f = 0; f < info.focusSamples; ++f)
        ////        lensSamples.Add(new LensSample(z, f, 0.0));

        zoomSamples = new double[info.zoomSamples];
        focusSamples = new double[info.focusSamples];
	}



    public bool Load(string filePath)
    {
        if (ReadXml(filePath, ref lensSamples))
        {
            return BuildLensTable();
        }

        return false;
    }

    

    public bool Save(string filePath, ArrayList samples)
    {
        return WriteXml(filePath, samples);
    }



    public float Fov(double zoom, double focus)
    {
        return (float)alglib.spline2d.spline2dcalc(splineLens, zoom, focus);
    }

   

    public void Reset()
    {
        if (info == null)
            info = new LensInfo();

        if (splineLens == null)
            splineLens = new alglib.spline2d.spline2dinterpolant();

        if (lensSamples == null)
            lensSamples = new ArrayList();
        
        // Clearing arraylists
        lensSamples.Clear();

        for (int z = 0; z < info.zoomSamples; ++z)
            for (int f = 0; f < info.focusSamples; ++f)
                lensSamples.Add(new LensSample(z, f, 0.0));

        zoomSamples = new double[info.zoomSamples];
        focusSamples = new double[info.focusSamples];
    }

    public void Clear()
    {
        // Clearing arraylists
        lensSamples.Clear();

        zoomSamples = new double[info.zoomSamples];
        focusSamples = new double[info.focusSamples];
    }

    

    public bool BuildLensTable(bool rebuildZoomFocusGuideSamples = false)
    {
        try
        {
            zoomMaxValue = 0;
            focusMaxValue = 0;

            if (rebuildZoomFocusGuideSamples)
            {
                ArrayList zs = new ArrayList();
                ArrayList fs = new ArrayList();
                foreach (LensSample ls in lensSamples)
                {
                    if (!zs.Contains(ls.zoom))
                    {
                        zs.Add(ls.zoom);

                        if (ls.zoom > zoomMaxValue)
                            zoomMaxValue = ls.zoom;
                    }

                    if (!fs.Contains(ls.focus))
                    {
                        fs.Add(ls.focus);

                        if (ls.focus > focusMaxValue)
                            focusMaxValue = ls.focus;
                    }
                }

                zoomSamples = (double[])zs.ToArray(typeof(double));
                focusSamples = (double[])fs.ToArray(typeof(double));
            }
            else
            {
                for (int ff = 0; ff < focusSamples.Length; ++ff)
                {
                    if (ff > focusMaxValue)
                        focusMaxValue = ff;

                    for (int zz = 0; zz < zoomSamples.Length; ++zz)
                    {
                        if (zz > zoomMaxValue)
                            zoomMaxValue = zz;

                        LensSample l = (LensSample)lensSamples[zz * focusSamples.Length + ff];
                        l.zoom = zoomSamples[zz];
                        l.focus = focusSamples[ff];
                    }
                }
            }



            int N = info.zoomSamples;
            int M = info.focusSamples;

            double[] zoom = new double[N];
            double[] focus = new double[M];
            double[,] fov = new double[M, N];

            int z = 0;
            int f = 0;
            int i = 0;

            foreach (LensSample l in lensSamples)
            {
                z = i / info.focusSamples;
                f = i % info.focusSamples;
                zoom[z] = (double)l.zoom;
                focus[f] = (double)l.focus;
                fov[f, z] = (double)l.fov;
                ++i;
            }

            alglib.spline2d.spline2dbuildbicubic(zoom, focus, fov, M, N, splineLens);
        }
        catch (System.Exception ex)
        {
            Debug.LogError("Failed trying to build lens table: " + ex.Message);
            return false;
        }
        return true;
    }

    

    [RPC]
    bool WriteXml(string file_path, ArrayList lens_table)
    {
        XmlDocument xmlDoc = new XmlDocument();

        XmlDeclaration xmlDec = xmlDoc.CreateXmlDeclaration("1.0", null, null);
        xmlDoc.AppendChild(xmlDec);

        XmlElement xmlLensTable = xmlDoc.CreateElement("LensTable");
        xmlDoc.AppendChild(xmlLensTable);

        XmlElement xmlLensDescription = xmlDoc.CreateElement("LensDescription");
        XmlIO.WriteFields(xmlLensDescription, info);
        xmlLensTable.AppendChild(xmlLensDescription);

        foreach (LensSample l in lens_table)
        {
            XmlElement xmlLensSample = xmlDoc.CreateElement("LensSample");
            xmlLensTable.AppendChild(xmlLensSample);

            XmlIO.WriteFields(xmlLensSample, l);
        }

        try
        {
            xmlDoc.Save(file_path);
        }
        catch (System.Exception e)
        {
            Debug.LogError(e.Message);
            return false;
        }
        return true;
    }




    bool ReadXml(string file_path, ref ArrayList lens_table)
    {
        XmlDocument xmlDoc = new XmlDocument();

        try
        {
            xmlDoc.Load(file_path);

            XmlNodeList lensDescription = xmlDoc.GetElementsByTagName("LensDescription");
            
            if (lensDescription != null)
            {
                foreach (XmlNode lensInfoNode in lensDescription)
                {
                    XmlIO.Read(lensInfoNode.ChildNodes[0], ref info.manufacturer);
                    XmlIO.Read(lensInfoNode.ChildNodes[1], ref info.model);
                    XmlIO.Read(lensInfoNode.ChildNodes[2], ref info.range);
                    XmlIO.Read(lensInfoNode.ChildNodes[3], ref info.patrimonyCode);
                    XmlIO.Read(lensInfoNode.ChildNodes[4], ref info.zoomSamples);
                    XmlIO.Read(lensInfoNode.ChildNodes[5], ref info.focusSamples);
                }
            }

            XmlNodeList lensEntryList = xmlDoc.GetElementsByTagName("LensSample");
         
            if (lensEntryList != null)
            {
                foreach (XmlNode lensEntryNode in lensEntryList)
                {
                    LensSample l = new LensSample();

                    XmlIO.Read(lensEntryNode.ChildNodes[0], ref l.zoom);
                    XmlIO.Read(lensEntryNode.ChildNodes[1], ref l.focus);
                    XmlIO.Read(lensEntryNode.ChildNodes[2], ref l.fov);

                    lens_table.Add(l);
                }
            }
        }
        catch (System.Exception e)
        {
            Debug.LogError(e.Message);
            return false;
        }
        return true;
    }



    

#if false
    void ReadLensFileTxt(string file_path)
    {
        try
        {
            // Create an instance of StreamReader to read from a file.
            // The using statement also closes the StreamReader.
            using (StreamReader sr = new StreamReader(file_path))
            {
                string line;
                // Read and display lines from the file until the end of 
                // the file is reached.
                while ((line = sr.ReadLine()) != null)
                {
                    ArrayList data = new ArrayList();
                    string[] strData = line.Split(' ');

                    foreach (string s in strData)
                    {
                        if (s.Length > 0)
                        {
                            data.Add(s);
                        }
                    }
                    LensEntry l = new LensEntry();
                    for (int i = 0; i < data.Count; i = i + 4)
                    {
                        l.zoom = int.Parse((string)data[i + 0]);
                        l.focus = int.Parse((string)data[i + 1]);
                        l.fov = float.Parse((string)data[i + 2]);
                    }
                    lensSamples.Add(l);
                }
            }
        }
        catch (Exception e)
        {
            // Let the user know what went wrong.
            Debug.LogError("The file could not be read: " + e.Message);
        }
    }
#endif

    static string[] SplitValues(string s)
    {
        //
        // Split on all non-word characters.
        // ... Returns an array of all the words.
        //
        return Regex.Split(s, @" * ");
        // @      special verbatim string syntax
        // \W+    one or more non-word characters together
    }
}
