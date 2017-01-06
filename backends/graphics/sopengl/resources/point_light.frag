#version 330

// clang-format off
#include <post_process_effect.glsl>
#include <lighting.glsl>
// clang-format on

in point_light
{
    vec3 position;
    float radius;
    vec3 color;
    float intensity;
} light;



void main()
{
    surface s = read_geometry_buffer();

    vec3 V = -normalize(s.position);
    vec3 L = light.position - s.position;
    float light_distance = length(L);
    L = normalize(L);

    // TODO Use eq (9)
    // http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf
    float att = smoothstep(1, 0, light_distance / light.radius) / (light_distance * light_distance);

    vec3 final_color = att * light.intensity * light.color * compute_lighting(s, L, V);

    out_image = vec4(final_color, 1);
}
