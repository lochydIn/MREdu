//
// Created by Lochlan Harvey on 24/01/2026.
//

#pragma once
#include <memory>
#include <string>
#include <vector>

class Component;

class ModelObject {
public:
// Constructor
        ModelObject();
        explicit ModelObject(const std::string & name);
// Deconstructor
        virtual ~ModelObject() = default;

//Key Methods
        virtual void update();

        virtual void render();

// Getters/Setters
        const std::string & getName() const
        {
                return m_Name;
        }

        void setName(const std::string & name)
        {
                m_Name = name;
        }

private:
// Variables
        std::string m_Name;
        std::vector<std::unique_ptr<Component>> m_components;

// Copy Prevention
        ModelObject(const ModelObject&) = delete;
        ModelObject& operator=(const ModelObject&) = delete;

};


