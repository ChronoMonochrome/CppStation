#version 330 core

layout(location = 0) in ivec2 position;
layout(location = 1) in ivec3 color;

out vec3 v_color;

void main() {
  ivec2 pos = position;

  // Convert VRAM coordinates (0;1023, 0;511) into OpenGL coordinates
  // (-1;1, -1;1)
  float xpos = (float(pos.x) / 512) - 1.0;
  // VRAM puts 0 at the top, OpenGL at the bottom, we must mirror
  // vertically
  float ypos = 1.0 - (float(pos.y) / 256);

  gl_Position.xyzw = vec4(xpos, ypos, 0.0, 1.0);

  // Glium doesn't support "normalized" for now
  v_color = color / 255;
}