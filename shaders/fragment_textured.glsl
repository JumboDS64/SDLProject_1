
uniform sampler2D diffuse;
varying vec2 texCoordVar;
in vec4 ourColor;

void main() {
    gl_FragColor = texture2D(diffuse, texCoordVar)*ourColor;
}
