/*
 * QtGLAppBase - Simple Qt Application to get started with OpenGL stuff.
 *
 * Copyright (c) 2011-2012 Martin Felis <martin.felis@iwr.uni-heidelberg.de>
 *
 * Licensed under the MIT license. See LICENSE for more details.
 */

#ifndef _OBJLOADER_H
#define _OBJLOADER_H

#include "MeshVBO.h"

bool load_obj (MeshVBO &mesh, const char *filename, bool strict = true);

// only loads the object in the OBJ file of the given obj_name
bool load_obj (MeshVBO &mesh, const char *filename, const char *sub_object_name, bool strict = true);


#endif
