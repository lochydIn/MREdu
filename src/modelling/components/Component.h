//
// Created by Lochlan Harvey on 24/01/2026.
//

#pragma once

class Component {
    public:
    // Constructor
    Component(const std::string & name) {
                m_name = name;
            }

    virtual ~Component() = default;

    private:
        std::string m_name;

};



