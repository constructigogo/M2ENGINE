//
// Created by constantin on 29/11/23.
//

#pragma once

#include "bgfx/bgfx.h"

namespace Engine {
    template<typename T>
    class GenericHandle {
    public:

        GenericHandle() = default;
        explicit GenericHandle(T _handle) : handle(_handle){

        };

        ~GenericHandle() {
            if (bgfx::isValid(handle)){
                bgfx::destroy(handle);
            }
        }

        GenericHandle& operator=(T _handle){
            swap(_handle);
        }

        void swap(T _handle){
            clean();
            handle=_handle;
        }

        operator T() const {
            return handle;
        }

    protected:
        T handle = BGFX_INVALID_HANDLE;
        void clean(){
            if (bgfx::isValid(handle)){
                bgfx::destroy(handle);
            }
        }
    private:
    };

    typedef bgfx::ProgramHandle ProgramHandle;
    typedef bgfx::UniformHandle UniformHandle;
    typedef bgfx::VertexBufferHandle VertexBufferHandle;

} // Engine
