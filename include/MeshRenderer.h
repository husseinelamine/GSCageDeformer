#ifndef MESHRENDERER_H
#define MESHRENDERER_H

#include <GL/glew.h>

class MeshRenderer {
public:
    virtual ~MeshRenderer() {}
    virtual void setupBuffers() = 0;
    virtual void render() = 0;
};

#endif // MESHRENDERER_H
