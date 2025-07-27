#ifndef WRE_TEXTURING_H__
#define WRE_TEXTURING_H__
#include <stdint.h>
typedef uint32_t Texture;

Texture upload_texture(char *path);
// // returns only one texture, you can assume that from this textures id to the id + texture_count - 1 are the textures you just uploaded
// Texture batch_upload_texture(char *path[], uint16_t texture_count);
#endif