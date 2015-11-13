#version 430

//layout(location = 0) uniform int            render_mode;
layout(location = 2) uniform sampler2D      Y_tex;
layout(location = 3) uniform sampler2D      Cr_tex;
layout(location = 4) uniform sampler2D      Cb_tex;

in  vec2 texcoords_frag;
out vec4 fragment_color;

void main() {

    float Y  = texture(Y_tex , vec2(texcoords_frag.x, 1 - texcoords_frag.y)).r;
    float Cr = texture(Cr_tex, vec2(texcoords_frag.x, 1 - texcoords_frag.y)).r;
    float Cb = texture(Cb_tex, vec2(texcoords_frag.x, 1 - texcoords_frag.y)).r;

    /* 
    float R = Y + 1.402   * (Cr - 128);
    float G = Y - 0.34414 * (Cb - 128) - 0.71414 * (Cr - 128);
    float B = Y + 1.772   * (Cb - 128);
    */
    
    /*
    float R = Y + 1.403 * Cr;
    float G = Y - 0.344 * Cb - 0.714 * Cr;
    float B = Y + 1.770 * Cb;
    */

    /*
    float C = Y  - 16;
    float D = Cb - 128;
    float E = Cr - 128;

    float R = (298 * C           + 409 * E + 128) / 256;
    float G = (298 * C - 100 * D - 208 * E + 128) / 256;
    float B = (298 * C + 516 * D           + 128) / 256;
    */

    /*
    float B = 1.164 * (Y - 16)                      + 2.018 * (Cb - 128);
    float G = 1.164 * (Y - 16) - 0.813 * (Cr - 128) - 0.391 * (Cb - 128);
    float R = 1.164 * (Y - 16) + 1.596 * (Cr - 128);
    B = clamp(B, 0, 255);
    G = clamp(G, 0, 255);
    R = clamp(R, 0, 255);
    */

    float R = Y + 1.370705 * (Cb - 0.5);
    float G = Y - 0.698001 * (Cr - 0.5) - 0.337633 * (Cb - 0.5);
    float B = Y + 1.732446 * (Cr - 0.5);

    fragment_color = vec4(R, G, B, 1);
}