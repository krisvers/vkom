#pragma once

#include <vector>

#include <vkom/object.hpp>

namespace vkom {

namespace internal {

class CollectedByHeap : virtual public ICollected {
private:
    uint32_t _referenceCount = 0;

public:
    CollectedByHeap() = default;
    virtual ~CollectedByHeap() = 0;

    uint32_t release() override;
    uint32_t retain() override;

protected:
    uint32_t referenceCount() const noexcept;
};

class DestructibleByHeap : virtual public IDestructible {
private:
    bool _deleted = false;

public:
    DestructibleByHeap() = default;
    virtual ~DestructibleByHeap() = 0;

    void destroy() override;
};

class DiscardableByHeap : virtual public IDiscardable {
private:
    bool _deleted = false;

public:
    DiscardableByHeap() = default;
    virtual ~DiscardableByHeap() = 0;

    void discard() override;
};

class ParentByVector : virtual public IParent {
private:
    std::vector<IChild*> _children = {};

public:
    ParentByVector() = default;
    virtual ~ParentByVector() = 0;

    bool hasChild(IChild const* child) const noexcept override;
    IChild* enumerateChildren(uint32_t id, IID const& filter) const noexcept override;
    bool adopt(IChild* child) noexcept override;
    bool disown(IChild* child) noexcept override;

protected:
    void disownAll() noexcept;
};

}

}
