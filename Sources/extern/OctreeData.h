/*
 * Copyright (c) 2018 Gabriel Vaduva
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */
#pragma once

#include "Vector3.h"
#include "MemoryPool.h"

namespace otr
{
    template<typename T = void>
    class OctreeData
    {
        friend class MemoryPool<OctreeData<T>>;
    public:
        Vector3 mPosition;
        // Custom user-data associated with this point
        T* mData;

        static OctreeData<T>* New(const Vector3& position, T* data = nullptr)
        {
            OctreeData<T>* node = mAllocator.newElement();
            node->mPosition = position;
            node->mData = data;
            return node;
        }

        void Delete()
        {
            mAllocator.deleteElement(this);
        }

    private:
        OctreeData() { }
        static MemoryPool<OctreeData<T>> mAllocator;
    };

#ifdef OCTREE_IMPL
    template<typename T>
    MemoryPool<OctreeData<T>> OctreeData<T>::mAllocator;
#endif
}