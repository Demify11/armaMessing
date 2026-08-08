#pragma once
#include <vector>
#include "Element.h"
class Group
{
	std::string m_Name;
	std::vector<Element*> m_Elements;
    float m_Provisional = 200.0f;
    float m_Measured = 0.0f;
    bool m_FullWidth = false;

public:

    explicit Group(std::string name) : m_Name(std::move(name)) {}

    ~Group() {
        for (auto* el : m_Elements)
            delete el;
    }

    Group& SetFullWidth(bool v = true) { m_FullWidth = v; return *this; }
    bool   IsFullWidth() const { return m_FullWidth; }

    // move: steal the buffer. vector's move leaves other.m_Elements EMPTY,
    // so the moved-from object's destructor deletes nothing — no double free.
    Group(Group&& o) noexcept
        : m_Name(std::move(o.m_Name)),
        m_Elements(std::move(o.m_Elements)),
        m_Provisional(o.m_Provisional),
        m_Measured(o.m_Measured),
        m_FullWidth(o.m_FullWidth)
    {}

    Group& operator=(Group&& o) noexcept {
        if (this != &o) {
            for (auto* el : m_Elements) delete el;   // free what we hold first
            m_Name = std::move(o.m_Name);
            m_Elements = std::move(o.m_Elements);
            m_Provisional = o.m_Provisional;
            m_Measured = o.m_Measured;
            m_FullWidth = o.m_FullWidth;
        }
        return *this;
    }

    // forbid copying — a shallow copy of owning raw pointers is the bug
    Group(const Group&) = delete;
    Group& operator=(const Group&) = delete;

    float Height();

    template <typename T, typename... Args>
    T& Add(Args&&... args) {
        T* e = new T(std::forward<Args>(args)...);
        m_Elements.push_back(e);
        return *e;;
    }

	void Draw(float width);


};

