attribute vec4 position;
attribute vec2 texCoord;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

varying vec2 texCoordVar;
out vec4 ourColor;

void main()
{
	vec4 p = viewMatrix * modelMatrix  * position;
    texCoordVar = texCoord;
	gl_Position = projectionMatrix * p;
	ourColor = vec4(1.0, 1.0 - position.y/128.0, 1.0, 1.0);
}