#pragma once

#if _DEBUG
#include "format.h"
#endif

#ifdef LAVA_EXPORTS
#define LAVA_API __declspec(dllexport)
#else
#define LAVA_API __declspec(dllimport)
#endif // LAVA_EXPORTS

#define HAS_STATE(integer, state) (((integer) & (state)) == (state))
#define ADD_STATE(integer, state) ((integer) |= (state))
#define REMOVE_STATE(integer, state) ((integer) &= ~(state))

#if _DEBUG
#define LOG_INFO(msg, ...) fmt::print(msg, __VA_ARGS__);
#define LOG_WARNING(msg, ...) fmt::print_colored(fmt::Color::YELLOW, msg, __VA_ARGS__);
#define LOG_ERROR(msg, ...) { fmt::print_colored(fmt::Color::RED, msg, __VA_ARGS__); }
#else
#define LOG_INFO(msg, ...)
#define LOG_WARNING(msg, ...)
#define LOG_ERROR(msg, ...)
#endif

#define THROW(msg, ...) throw std::runtime_error(fmt::format(msg, __VA_ARGS__))
#define THROW_IF(condition, msg, ...) if ((condition)) throw std::runtime_error(fmt::format(msg, __VA_ARGS__))

#define MAX_TASKS_PER_FRAME 16
#define MAX_BUFFER_COPY_PER_FRAME 128
constexpr float TIME_STEP = 1.f / 60.f;
//#define SINGLE_THREAD

#define MEM_POOL_DECLARE(Type) static Type* Allocate() { return mAllocator.newElement(); } \
private: \
static MemoryPool<Type> mAllocator

#define MEM_POOL_DECLARE_SIZE(Type, BlockSize) static Type* Allocate() { return mAllocator.newElement(); } \
private: \
static MemoryPool<Type, (BlockSize)> mAllocator

#define MEM_POOL_DEFINE(Type) MemoryPool<Type> Type::mAllocator

#define MEM_POOL_DEFINE_SIZE(Type, BlockSize) MemoryPool<Type, (BlockSize)> Type::mAllocator

namespace Engine
{
    typedef void(*UpdateCback)();
    typedef void(*FramebufferResizeCback)(int, int);

    enum class PipelineType
    {
        Unknown, Default, Albedo, Phong
    };
}