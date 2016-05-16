using UnityEngine;
using System.Collections;
using System.Reflection;
using System.Xml;
using System;

public class XmlIO
{

	static string UnityType(System.Type unityType)
    {
        return unityType.ToString().Replace("UnityEngine.", "");
    }
    

    public static void ReadHierarchy(string file_path, GameObject rootObj)
    {
        Transform[] nodes = rootObj.GetComponentsInChildren<Transform>(true);
        foreach (Transform t in nodes)
        {
            Read(file_path + t.name + ".xml", t.gameObject);
        }
    }


    public static void WriteHierarchy(string file_path, string file_extension, GameObject rootObj)
    {
        Transform[] nodes = rootObj.GetComponentsInChildren<Transform>(true);
        foreach (Transform t in nodes)
        {
            Write(file_path + t.name + file_extension, t.gameObject);
        }
    }


    static void XmlTransform(XmlNode parent, Transform transf)
    {
        foreach (XmlNode nodeProperty in parent.ChildNodes)
        {
            PropertyInfo propInfo = transf.GetType().GetProperty(nodeProperty.Attributes[0].Value);

            if (nodeProperty.Name != "Vector3")
            {
                continue;
            }

            Vector3 vec = LoadVector3(nodeProperty);
            propInfo.SetValue(transf, vec, null);
        }
    }


    static Vector3 LoadVector3(XmlNode vecXml)
    {
        Vector3 vec = Vector3.zero;

        int i = 0;
        foreach (XmlNode nodeField in vecXml.ChildNodes)
        {
            string fieldName = nodeField.Attributes[1].Value;
            string fieldValue = nodeField.Attributes[2].Value;

            FieldInfo f = vec.GetType().GetField(fieldName);
            if (f == null)
            {
                Debug.LogError("Field <" + fieldName + "> not found");
            }
            else
            {
                vec[i++] = System.Single.Parse(fieldValue);
            }
        }

        return vec;
    }

    public static void Read(string filePath, GameObject gameObj)
    {
        XmlDocument xmlDoc = new XmlDocument();
        
        try
        {
            xmlDoc.Load(filePath);

            XmlNodeList gameObjNodeList = xmlDoc.GetElementsByTagName(gameObj.name);

            if (gameObjNodeList != null)
            {
                foreach (XmlNode gameObjNode in gameObjNodeList)
                {
                    foreach (XmlNode nodeComponent in gameObjNode.ChildNodes)
                    {
                        if(nodeComponent.Name == "Transform")
                        {
                            XmlTransform(nodeComponent, gameObj.GetComponent("Transform") as Transform);
                        }

                    }
                }
            }
        }
        catch (System.Exception e)
        {
            Debug.LogError(e.Message);
        }
    }


    static void LoadObject(XmlNode vecXml, ref object obj)
    {
        int i = 0;
        foreach (XmlNode nodeField in vecXml.ChildNodes)
        {            
            string fieldName = nodeField.Attributes[1].Value;
            string fieldValue = nodeField.Attributes[2].Value;

            FieldInfo f = obj.GetType().GetField(fieldName);
            if (f == null)
            {
                Debug.LogError("Field <" + fieldName + "> not found");
            }
            else
            {
                f.SetValue(obj, i);
                obj = System.Single.Parse(fieldValue);
            }
        }
    }

    
    public static void Write(string file_path, GameObject gameObj)
    {
        XmlDocument xmlDoc = new XmlDocument();

        XmlDeclaration xmlDec = xmlDoc.CreateXmlDeclaration("1.0", null, null);
        xmlDoc.AppendChild(xmlDec);

        XmlElement xmlGameObject = xmlDoc.CreateElement(gameObj.name);
        xmlDoc.AppendChild(xmlGameObject);
        
        XmlElement xmlTranform = XmlComponent(xmlGameObject, gameObj.transform);
        XmlPropertyInfo(xmlTranform, gameObj.transform.GetType().GetProperty("localPosition"), gameObj.transform);
        XmlPropertyInfo(xmlTranform, gameObj.transform.GetType().GetProperty("localEulerAngles"), gameObj.transform);
        XmlPropertyInfo(xmlTranform, gameObj.transform.GetType().GetProperty("localScale"), gameObj.transform);


        try
        {
            xmlDoc.Save(file_path); 
        }
        catch (System.Exception e)
        {
            Debug.LogError(e.Message);
        }
    }


    static XmlElement XmlComponent(XmlElement parent, Component comp)
    {
        XmlElement compXml = parent.OwnerDocument.CreateElement(UnityType(comp.GetType()));
        parent.AppendChild(compXml);
        return compXml;
    }



    static XmlElement XmlPropertyInfo(XmlElement parent, PropertyInfo propInfo, object obj)
    {
        XmlElement propXml = parent.OwnerDocument.CreateElement(UnityType(propInfo.PropertyType));
        parent.AppendChild(propXml);

        propXml.SetAttribute("name", propInfo.Name.ToString());
        
        object v = propInfo.GetValue(obj, null);
        foreach (FieldInfo f in v.GetType().GetFields())
        {
            if(!f.IsLiteral)
                XmlUnityFieldInfo(propXml, f, v);
        }
        return propXml;
    }



    public static XmlElement XmlUnityFieldInfo(XmlElement parent, FieldInfo fieldInfo, object obj)
    {
        XmlElement fieldXml = parent.OwnerDocument.CreateElement("field");
        fieldXml.SetAttribute("type", UnityType(fieldInfo.FieldType));
        fieldXml.SetAttribute("name", fieldInfo.Name);
        fieldXml.SetAttribute("value", fieldInfo.GetValue(obj).ToString());
        parent.AppendChild(fieldXml);
        return fieldXml;
    }

    public static XmlElement WriteField(XmlElement parent, FieldInfo fieldInfo, object obj)
    {
        XmlElement fieldXml = parent.OwnerDocument.CreateElement("field");
        fieldXml.SetAttribute("type", fieldInfo.FieldType.ToString());
        fieldXml.SetAttribute("name", fieldInfo.Name);
        fieldXml.SetAttribute("value", fieldInfo.GetValue(obj).ToString());
        parent.AppendChild(fieldXml);
        return fieldXml;
    }



    public static void WriteFields(XmlElement parent, object obj)
    {
        foreach (FieldInfo f in obj.GetType().GetFields())
        {
            if (!f.IsLiteral)
                WriteField(parent, f, obj);
        }
    }


    public static void Write<T>(XmlElement parent, T obj, string obj_name)
    {
        XmlElement fieldXml = parent.OwnerDocument.CreateElement("attr");
        fieldXml.SetAttribute("type", obj.GetType().ToString());
        fieldXml.SetAttribute("name", obj_name);
        fieldXml.SetAttribute("value", obj.ToString());
        parent.AppendChild(fieldXml);
    }

      

    public static object ReadField(XmlNode nodeField)
    {
        object obj = System.Activator.CreateInstance(Type.GetType(nodeField.Attributes[0].Value));
        obj = nodeField.Attributes[2].Value;
        return obj;
    }


    public static void Read<T>(XmlNode nodeField, ref T obj)
    {
        obj = (T)System.Convert.ChangeType(nodeField.Attributes[2].Value, typeof(T));
    }


    public static T GetReference<T>(object inObj, string fieldName) where T : class
    {
        return GetField(inObj, fieldName) as T;
    }

    public static T GetValue<T>(object inObj, string fieldName) where T : struct
    {
        return (T)GetField(inObj, fieldName);
    }

    public static void SetField(object inObj, string fieldName, object newValue)
    {
        FieldInfo info = inObj.GetType().GetField(fieldName);
        if (info != null)
            info.SetValue(inObj, newValue);
    }

    private static object GetField(object inObj, string fieldName)
    {
        object ret = null;
        FieldInfo info = inObj.GetType().GetField(fieldName);
        if (info != null)
            ret = info.GetValue(inObj);
        return ret;
    }



    public static void WriteTransform(XmlElement xmlParent, UnityEngine.Transform transform)
    {
        XmlDocument xmlDoc = xmlParent.OwnerDocument;

        XmlElement xmlLocalPosition = xmlDoc.CreateElement("LocalPosition");
        XmlIO.WriteFields(xmlLocalPosition, transform.localPosition);
        xmlParent.AppendChild(xmlLocalPosition);

        XmlElement xmlEulerAngles = xmlDoc.CreateElement("EulerAngles");
        XmlIO.WriteFields(xmlEulerAngles, transform.localEulerAngles);
        xmlParent.AppendChild(xmlEulerAngles);

        XmlElement xmlLocalScale = xmlDoc.CreateElement("LocalScale");
        XmlIO.WriteFields(xmlLocalScale, transform.localScale);
        xmlParent.AppendChild(xmlLocalScale);
    }

    public static void ReadTransform(XmlNode xmlNode, ref UnityEngine.Transform transform)
    {
        Vector3 vec = Vector3.zero;

        XmlNode localPosNode = xmlNode.ChildNodes[0];
        XmlIO.Read(localPosNode.ChildNodes[0], ref vec.x);
        XmlIO.Read(localPosNode.ChildNodes[1], ref vec.y);
        XmlIO.Read(localPosNode.ChildNodes[2], ref vec.z);
        transform.localPosition = vec;

        XmlNode localEulerNode = xmlNode.ChildNodes[1];
        XmlIO.Read(localEulerNode.ChildNodes[0], ref vec.x);
        XmlIO.Read(localEulerNode.ChildNodes[1], ref vec.y);
        XmlIO.Read(localEulerNode.ChildNodes[2], ref vec.z);
        transform.localEulerAngles = vec;

        XmlNode localScaleNode = xmlNode.ChildNodes[2];
        XmlIO.Read(localScaleNode.ChildNodes[0], ref vec.x);
        XmlIO.Read(localScaleNode.ChildNodes[1], ref vec.y);
        XmlIO.Read(localScaleNode.ChildNodes[2], ref vec.z);
        transform.localScale = vec;
    }
}
