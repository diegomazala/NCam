Shader "Diem/TexelCoord" {
	Properties 
	{
		_MainTex("Main Texture (RGBA)", 2D) = "white" {}
	}


	SubShader 
	{
		Pass 
		{
			Name "TexelCoord"
			ZTest Always Cull Off ZWrite Off
			Fog { Mode off }

			CGPROGRAM
			#pragma target 3.0
			#pragma vertex vert_img
			#pragma fragment frag
			#pragma fragmentoption ARB_precision_hint_fastest 
			#include "UnityCG.cginc"
						
			uniform sampler2D _MainTex;
			uniform float4 _TexCoord;
 
			float4 frag (v2f_img i) : COLOR
			{
				float x = _TexCoord.x;
				float y = 1.0f - _TexCoord.y;
				return tex2D(_MainTex,float2(y,x));
			}
			ENDCG
		}
		
	}

	Fallback Off
} 
