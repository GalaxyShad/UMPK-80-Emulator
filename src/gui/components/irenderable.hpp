#ifndef IRENDERABLE_HPP
#define IRENDERABLE_HPP

class IRenderable {
public:
    virtual void render() = 0;
    virtual ~IRenderable() {}
};

#endif // IRENDERABLE_HPP
