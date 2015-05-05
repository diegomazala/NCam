Shader "NCam/Distortion" 
{
	Properties 
	{
		_MainTex ("Base (RGB)", 2D) = "" {}

	}
	SubShader 
	{
		Pass 
		{ 
			ZTest Always Cull Off ZWrite Off
			Fog { Mode off }      
 
			GLSLPROGRAM

			    uniform sampler2D _MainTex;
				uniform sampler2D DistortTex;

				uniform float DistortUScale;
				uniform float DistortUOffset;
				uniform float DistortVScale;
				uniform float DistortVOffset;

				#pragma fragmentoption ARB_precision_hint_fastest 

				#ifdef VERTEX
 
					void main()
					{                                         
						gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
						gl_TexCoord[0] = gl_MultiTexCoord0;
					}
 
				#endif	
 
				#ifdef FRAGMENT
 
					void main()
					{
						// When getting the distorted parameters, the distortion map orig is top left; opengl orig is bottom left
						vec2 lTexCoordImageOrig = vec2(gl_TexCoord[0].s, gl_TexCoord[0].t);
						lTexCoordImageOrig.x = lTexCoordImageOrig.x*DistortUScale + DistortUOffset;
						lTexCoordImageOrig.y = lTexCoordImageOrig.y*DistortVScale + DistortVOffset;
						vec2 lTexCoord = texture2D(DistortTex,lTexCoordImageOrig).rg;
						//lTexCoord.y = 1.0 - lTexCoord.y;
						gl_FragColor = texture2D(_MainTex, lTexCoord);
					}
 
				#endif
 
			ENDGLSL
		}
	}


	Fallback off
	
} // shader