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
				//return tex2D (_MainTex, i.uv) * tex2D (_MainTex, i.uv).a;
				
				//float2 uv = float2(_TexCoord.x, 1.0f - _TexCoord.y);
				//float4 c = tex2D(_MainTex, uv);
				float4 c = tex2D(_MainTex, _TexCoord.yx);

				//return float4(c.r, 0, 0, 0);
				//return float4(0, c.g, 0, 0);
				//return float4(0, 0, c.b, 0);
				//return float4(0, 0, 0, c.a);
				return c;
			}
			ENDCG
		}
		
	}

	Fallback Off
} 
