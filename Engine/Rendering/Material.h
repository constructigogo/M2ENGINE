//
// Created by constantin on 25/09/23.
//

#pragma once

#include "bgfx/bgfx.h"
#include <unordered_map>
#include <memory>
#include "Texture.h"
#include "glm/glm.hpp"

namespace Engine {

    class MaterialInstance;

    class Material {
    public:

        Material(const std::string &name, const bgfx::ProgramHandle &handle);
        Material(const std::string &name, const std::string& vertex,const std::string& fragment);

        std::shared_ptr<MaterialInstance> createInstance();

        const bgfx::ProgramHandle &getHandle() const;

        const std::string &getName() const;

        static Material Default;
        static Material Debug;

    protected:
        Material()=default;

        std::string name;
        bgfx::ProgramHandle handle;
    private:
    };


    class MaterialInstance{
    public:
        explicit MaterialInstance(const Material& from);
        void setTexture(Texture::TYPE type, std::shared_ptr<Texture> tex);
        void setTexture(const std::shared_ptr<Texture>&  tex);


        void setSpecColor(const glm::vec3 &specColor);

        void setSpecularStr(float specularStr);

        bool useMap(Texture::TYPE type) const;
        std::shared_ptr<Texture> getMap(Texture::TYPE type);
        const bgfx::ProgramHandle &getHandle() const;



        glm::vec4 getSpecular();

    private:
        glm::vec3 diffuse;
        glm::vec3 specColor;
        glm::vec3 emissiveColor;
        float specularStr;

        std::unordered_map<Texture::TYPE,std::shared_ptr<Texture>> textures;
        bgfx::ProgramHandle handle;
    };


} // Engine
