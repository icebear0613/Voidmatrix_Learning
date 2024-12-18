#pragma once

#include "timer.h"
#include "state_node.h"

class PlayerAttackState : public StateNode
{
public:
    PlayerAttackState();
    ~PlayerAttackState() = default;

    void on_entry() override;
    void on_update(float delta) override;
    void on_exit() override;

private:
    Timer timer;

private:
    void update_hit_box_position();

};

class PlayerDeadState : public StateNode
{
public:
    PlayerDeadState();
    ~PlayerDeadState() = default;

    void on_entry() override;
    void on_update(float delta) override;

private:
    Timer timer;

};

class PlayerFallState : public StateNode
{
public:
    PlayerFallState();
    ~PlayerFallState() = default;

    void on_entry() override;
    void on_update(float delta) override;

};

class PlayerIdleState : public StateNode
{
public:
    PlayerIdleState();
    ~PlayerIdleState() = default;

    void on_entry() override;
    void on_update(float delta) override;

};

class PlayerJumpState : public StateNode
{
public:
    PlayerJumpState() = default;
    ~PlayerJumpState() = default;

    void on_entry() override;
    void on_update(float delta) override;
};

class PlayerRollState : public StateNode
{
public:
    PlayerRollState();
    ~PlayerRollState() = default;

    void on_entry() override;
    void on_update(float delta) override;
    void on_exit() override;

private:
    Timer timer;

};

class PlayerRunState : public StateNode
{
public:
    PlayerRunState();
    ~PlayerRunState() = default;

    void on_entry() override;
    void on_update(float delta) override;
    void on_exit() override;

};