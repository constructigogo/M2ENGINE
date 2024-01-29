//
// Created by constantin on 25/09/23.
//

#include "Material.h"

#include <utility>
#include "../Core/Data.h"

namespace Engine {

    Material Material::Default;
    Material Material::Debug;


    const bgfx::ProgramHandle &Material::getHandle() const {
        return handle;
    }

    Material::Material(const std::string &name, const bgfx::ProgramHandle &handle) : name(name), handle(handle) {}

    Material::Material(const std::string &name, const std::string &vertex, const std::string &fragment) : name(name){
        handle = Data::loadProgram(vertex.c_str(),fragment.c_str());
    }

    std::shared_ptr<MaterialInstance> Material::createInstance() {
        return std::make_shared<MaterialInstance>(*this);
    }

    const std::string &Material::getName() const {
        return name;
    }


    void MaterialInstance::setTexture(Texture::TYPE type, std::shared_ptr<Texture> tex) {
        textures[type]=std::move(tex);
    }

    void MaterialInstance::setTexture(const std::shared_ptr<Texture>& tex) {
        textures[tex->getType()]=tex;
    }

    bool MaterialInstance::useMap(Texture::TYPE type) const {
        return textures.contains(type);
    }

    MaterialInstance::MaterialInstance(const Material &from) {
        handle=from.getHandle();
    }

    const bgfx::ProgramHandle &MaterialInstance::getHandle() const {
        return handle;
    }

    std::shared_ptr<Texture> MaterialInstance::getMap(Texture::TYPE type) {
        return textures[type];
    }

    glm::vec4 MaterialInstance::getSpecular() {
        return glm::vec4(specColor,specularStr);
    }

    void MaterialInstance::setSpecColor(const glm::vec3 &specColor) {
        MaterialInstance::specColor = specColor;
    }

    void MaterialInstance::setSpecularStr(float specularStr) {
        MaterialInstance::specularStr = specularStr;
    }


} // Engine