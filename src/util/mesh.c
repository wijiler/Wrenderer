#define CGLTF_IMPLEMENTATION
#include <cgltf/cgltf.h>
#include <stdio.h>
#include <util/mesh.h>

WREmesh loadMeshFromGLTF(char *filepath)
{
    WREmesh mesh = {0};
    cgltf_options meshoptions = {0};
    cgltf_data *meshdata = NULL;
    cgltf_result meshresult = cgltf_parse_file(&meshoptions, filepath, &meshdata);

    if (meshresult != cgltf_result_success)
    {
        printf("Could not load gltf %s E%i\n", filepath, meshresult);
        return mesh;
    }
    if (meshdata->meshes_count <= 0)
    {
        printf("No mesh contained in gltf file %s\n", filepath);
        return mesh;
    }
    cgltf_load_buffers(&meshoptions, meshdata, filepath);
    cgltf_mesh gltfMesh = meshdata->meshes[0];
    for (uint32_t i = 0; i < gltfMesh.primitives_count; i++)
    {
        if (gltfMesh.primitives[i].type == cgltf_primitive_type_triangles)
        {
            cgltf_primitive prim = gltfMesh.primitives[i];
            mesh.indices = malloc(sizeof(uint32_t) * prim.indices[0].count);
            uint16_t *Idat = (uint16_t *)prim.indices->buffer_view->buffer->data;
            for (uint32_t i = 0; i < prim.indices[0].count; i++)
            {
                mesh.indices[i] = Idat[i];
            }
            mesh.vertices = malloc(sizeof(WREVertex3D) * prim.indices[0].count);
            for (uint32_t i = 0; i < prim.attributes_count; i++)
            {
                cgltf_attribute attrib = prim.attributes[i];
                switch (attrib.type)
                {
                case cgltf_attribute_type_position:
                {
                    vec3 *data = (vec3 *)attrib.data->buffer_view->buffer->data;
                    for (uint32_t i = 0; i < attrib.data->count; i++)
                    {
                        mesh.vertices[i].pos = data[i];
                    }
                }
                break;
                case cgltf_attribute_type_color:
                {
                    vec4 *data = (vec4 *)attrib.data->buffer_view->buffer->data;
                    for (uint32_t i = 0; i < attrib.data->count; i++)
                    {
                        mesh.vertices[i].color = data[i];
                    }
                }
                break;
                case cgltf_attribute_type_texcoord:
                {
                    vec2 *data = (vec2 *)attrib.data->buffer_view->buffer->data;
                    for (uint32_t i = 0; i < attrib.data->count; i++)
                    {
                        mesh.vertices[i].uv = data[i];
                    }
                }
                break;
                default:
                    break;
                }
            }

            break;
        }
    }
    /* TODO Texture*/
    return mesh;
}