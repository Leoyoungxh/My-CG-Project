#version 330 core

in vec4 color;
in vec2 texCoord;
in vec3 normal;
in vec3 faceIndecies;

in vec3 pos;
in vec3 l_pos;

out vec4 fColor;
out vec4 fNormal;

uniform sampler2D texture;
uniform vec4 shadow;
uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct;
uniform float Shininess;

void main()
{
	// fColor = vec4(1, 0, 0, 0 ) * texture2D( texture, texCoord );
    //fColor = texture2D( texture, texCoord );
    // fColor = color;
    // fColor = vec4(0, 0, 0, 0 );

    // 计算四个归一化的向量 N,V,L,R(或半角向量H)
    
    vec3 N = normalize(normal);
    vec3 L = normalize(l_pos-pos);
    vec3 V = normalize(-pos);
    vec3 R = reflect(-L,N);

    // 环境光分量I_a
    vec4 I_a = AmbientProduct;

    // 计算漫反射系数alpha和漫反射分量I_d
    float alpha = max(dot(L,N),0);
    vec4 I_d = alpha*DiffuseProduct;

    // 计算高光系数beta和镜面反射分量I_s
    float beta = max(pow(dot(R,V),Shininess),0);
    vec4 I_s = beta * SpecularProduct;

    // 注意如果光源在背面则去除高光
    if( dot(L, N) < 0.0 ) {
        I_s = vec4(0.0, 0.0, 0.0, 1.0);
    }
    
    // @TODO: 计算最终每个片元的输出颜色
    fColor = (I_a + I_d + I_s)*0.5 + texture2D( texture, texCoord )*0.5;

    //fColor.a = 1.0;
}

