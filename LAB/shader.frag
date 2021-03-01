//#version 150 core
//
//out vec4 out_Color;
//
//void main(void)
//{
//	out_Color = vec4(1.0, 0.0, 0.0, 1.0);
//}

#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

// texture sampler
uniform sampler2D texture1;

void main()
{
    FragColor = texture(texture1, TexCoord);
}
