#include <vkom/internal/object.hpp>

namespace vkom {

namespace internal {

CollectedByHeap::~CollectedByHeap() = default;

uint32_t CollectedByHeap::release() {
    if (_referenceCount == 0) {
        /* very likely currently destructing */
        return 0;
    }

    _referenceCount -= 1;
    if (_referenceCount == 0) {
        delete this;
        return 0;
    }

    return _referenceCount;
}

uint32_t CollectedByHeap::retain() {
    _referenceCount += 1;
    return _referenceCount;
}

uint32_t CollectedByHeap::referenceCount() const noexcept {
    return _referenceCount;
}

DestructibleByHeap::~DestructibleByHeap() = default;

void DestructibleByHeap::destroy() {
    /* in order to prevent recursive destruction */
    if (_deleted) {
        return;
    }

    _deleted = true;
    delete this;
}

DiscardableByHeap::~DiscardableByHeap() = default;

void DiscardableByHeap::discard() {
    /* in order to prevent recursive destruction */
    if (_deleted) {
        return;
    }

    _deleted = true;
    delete this;
}

ParentByVector::~ParentByVector() = default;

bool ParentByVector::hasChild(IChild const* child) const noexcept {
    if (child == nullptr) {
        return false;
    }

    for (IChild const* c : _children) {
        if (c == child) {
            return true;
        }
    }

    return false;
}

IChild* ParentByVector::enumerateChildren(uint32_t id, IID const& filter) const noexcept {
    if (id >= _children.size()) {
        return nullptr;
    }

    if (filter == IID::null() || filter == IChild::iid() || filter == IBase::iid()) {
        return _children[id];
    }

    uint32_t currentID = 0;
    for (IChild* child : _children) {
        if (child->queryInterface(filter) != nullptr) {
            if (currentID == id) {
                return child;
            }

            currentID += 1;
            if (currentID > id) {
                return nullptr;
            }
        }
    }

    return nullptr;
}

bool ParentByVector::adopt(IChild* child) noexcept {
    if (child == nullptr) {
        return false;
    }

    if (child->parent() != this) {
        return false;
    }

    if (hasChild(child)) {
        return false;
    }

    _children.push_back(child);
    ICollected* collected = child->queryInterface<ICollected>();
    if (collected != nullptr) {
        collected->retain();
    }

    return true;
}

bool ParentByVector::disown(IChild* child) noexcept {
    if (child == nullptr) {
        return false;
    }

    if (child->parent() != this) {
        return false;
    }

    for (auto it = _children.begin(); it != _children.end(); ++it) {
        if (*it == child) {
            _children.erase(it);

            ICollected* collected = child->queryInterface<ICollected>();
            if (collected != nullptr) {
                collected->release();
            }

            IDestructible* destructible = child->queryInterface<IDestructible>();
            if (destructible != nullptr) {
                destructible->destroy();
            }

            IDiscardable* discardable = child->queryInterface<IDiscardable>();
            if (discardable != nullptr) {
                discardable->discard();
            }

            return true;
        }
    }

    return false;
}

void ParentByVector::disownAll() noexcept {
    while (disown(enumerateChildren(0, IID::null()))) {}
}

}

}
