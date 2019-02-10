/*
 * Copyright (c) 2018 Gabriel Vaduva
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */
#pragma once

#include "OctreeData.h"
#include <vector>

#define SPLIT_DIM(dim, i, k) dim * ((i & k) ? 0.5f : -0.5f)

namespace otr
{
    bool IsPointInsideBox(const Vector3& p, const Vector3& bmin, const Vector3& bmax)
    {
        if (p.x <= bmax.x && p.y <= bmax.y && p.z <= bmax.z &&
            p.x >= bmin.x && p.y >= bmin.y && p.z >= bmin.z)
            return true;
        return false;
    }

    bool IsIntersecting(const Vector3& amin, const Vector3& amax,
        const Vector3& bmin, const Vector3& bmax)
    {
        if (amax.x >= bmin.x && amax.y >= bmin.y && amax.z >= bmin.z &&
            amin.x <= bmax.x && amin.y <= bmax.y && amin.z <= bmax.z)
            return true;
        return false;
    }

    template<typename T = void>
    class Octree
    {
        friend class MemoryPool<Octree<T>>;
    public:
        /*
         *  - means less than 'origin' in that dimension, + means greater than.
         *  child: 0 1 2 3 4 5 6 7
         *  x: - - - - + + + +
         *  y: - - + + - - + +
         *  z: - + - + - + - +
         */

        static Octree<T>* New(const Vector3& origin, const Vector3& extent)
        {
            Octree<T>* octree = mAllocator.newElement();
            octree->mOrigin = origin;
            octree->mExtent = extent;
            return octree;
        }

        void Delete()
        {
            for (auto& child : mChild)
            {
                if (child)
                {
                    child->Delete();
                }
                child = nullptr;
            }
            if (mData)
            {
                mData->Delete();
                mData = nullptr;
            }
            mAllocator.deleteElement(this);
        }

        int GetOctantContainingPoint(const Vector3& point) const
        {
            int octant = 0;
            if (point.x >= mOrigin.x) octant |= 4;
            if (point.y >= mOrigin.y) octant |= 2;
            if (point.z >= mOrigin.z) octant |= 1;
            return octant;
        }

        bool IsLeaf() const
        {
            return mChild[0] == nullptr;
        }

        void Insert(OctreeData<T>* dataPoint)
        {
            if (IsLeaf())
            {
                if (mData == nullptr)
                {
                    mData = dataPoint;
                }
                else
                {
                    OctreeData<T>* oldPoint = mData;
                    mData = nullptr;

                    for (int i = 0; i < OCT_SIZE; ++i)
                    {
                        Vector3 newOrig = mOrigin;
                        newOrig.x += SPLIT_DIM(mExtent.x, i, 4);
                        newOrig.y += SPLIT_DIM(mExtent.y, i, 2);
                        newOrig.z += SPLIT_DIM(mExtent.z, i, 1);
                        mChild[i] = Octree::New(newOrig, mExtent * 0.5f);
                    }

                    mChild[GetOctantContainingPoint(oldPoint->mPosition)]->Insert(oldPoint);
                    mChild[GetOctantContainingPoint(dataPoint->mPosition)]->Insert(dataPoint);
                }
            }
            else
            {
                mChild[GetOctantContainingPoint(dataPoint->mPosition)]->Insert(dataPoint);
            }
        }
        
        void GetPointsInsideBox(const Vector3& bmin, const Vector3& bmax,
            std::vector<OctreeData<T>*>& points) const
        {
            if (IsLeaf())
            {
                if (mData != nullptr && IsPointInsideBox(mData->mPosition, bmin, bmax))
                {
                    points.push_back(mData);
                }
            }
            else
            {
                for (int i = 0; i < OCT_SIZE; ++i)
                {
                    Vector3 cmax = mChild[i]->mOrigin + mChild[i]->mExtent;
                    Vector3 cmin = mChild[i]->mOrigin - mChild[i]->mExtent;
                    
                    if (IsIntersecting(cmin, cmax, bmin, bmax))
                    {
                        mChild[i]->GetPointsInsideBox(bmin, bmax, points);
                    }
                }
            }
        }
    private:
        Octree() : mData(nullptr)
        {
            for (auto& child : mChild)
                child = nullptr;
        }

        static constexpr int OCT_SIZE = 8;

        Vector3 mOrigin; // center of this octree
        Vector3 mExtent; // half the dimension

        Octree<T>* mChild[OCT_SIZE];
        OctreeData<T>* mData;

        static MemoryPool<Octree<T>> mAllocator;
    };

#ifdef OCTREE_IMPL
    template<typename T>
    MemoryPool<Octree<T>> Octree<T>::mAllocator;
#endif

#undef SPLIT_DIM
}