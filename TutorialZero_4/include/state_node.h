#pragma once

class StateNode
{
public:
    StateNode() = default;
    ~StateNode() = default;

    virtual void on_entry() { };
    virtual void on_update(float delta) { };
    virtual void on_exit() { };
};