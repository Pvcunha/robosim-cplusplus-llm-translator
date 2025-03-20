#include "rcsc/geom/vector_2d.h"
#include <boost/statechart/custom_reaction.hpp>
#include <boost/statechart/termination.hpp>
#include <boost/statechart/event.hpp>
#include <boost/statechart/result.hpp>
#include <boost/statechart/simple_state.hpp>
#include <boost/statechart/state.hpp>
#include <boost/statechart/state_machine.hpp>
#include <boost/statechart/transition.hpp>
#include <rcsc/player/soccer_action.h>
#include <rcsc/player/world_model.h>
#include <rcsc/player/player_agent.h>

namespace sc = boost::statechart;

bool isCatchable(rcsc::PlayerAgent *agent);
bool isInOurPenaltyArea(rcsc::PlayerAgent *agent);
bool checkTackle(double prob);

struct EvProcess : sc::event<EvProcess> {};

class StateMachine : public sc::state_machine<StateMachine, struct i0> {
public:
    bool insideGoalieArea;
    bool catchable;
    bool isTacklePossible;
    rcsc::PlayerAgent *agent;
};

struct i0 : sc::state<i0, StateMachine> {
    typedef sc::custom_reaction<EvProcess> reactions;
    i0(my_context ctx) : my_base(ctx) {}
    sc::result react(const EvProcess &) {
        return transit<SdoCatch>();
    }
};

struct SdoCatch : sc::state<SdoCatch, StateMachine> {
    typedef sc::custom_reaction<EvProcess> reactions;
    SdoCatch(my_context ctx) : my_base(ctx) {
        StateMachine &machine = context<StateMachine>();
        machine.insideGoalieArea = isInOurPenaltyArea(machine.agent);
        machine.catchable = isCatchable(machine.agent);
    }
    sc::result react(const EvProcess &) {
        StateMachine &machine = context<StateMachine>();
        if (machine.catchable && machine.insideGoalieArea) {
            machine.agent->doShoot();
            return transit<f0>();
        }
        return transit<SClearBall>();
    }
};

struct SClearBall : sc::state<SClearBall, StateMachine> {
    typedef sc::custom_reaction<EvProcess> reactions;
    SClearBall(my_context ctx) : my_base(ctx) {}
    sc::result react(const EvProcess &) {
        StateMachine &machine = context<StateMachine>();
        if (machine.agent->world().self().isKickable()) {
            machine.agent->doTackle();
            return transit<f0>();
        }
        return transit<SdoTackle>();
    }
};

struct SdoTackle : sc::state<SdoTackle, StateMachine> {
    typedef sc::custom_reaction<EvProcess> reactions;
    SdoTackle(my_context ctx) : my_base(ctx) {
        StateMachine &machine = context<StateMachine>();
        machine.isTacklePossible = checkTackle(machine.agent->world().self().tackleProbability());
    }
    sc::result react(const EvProcess &) {
        StateMachine &machine = context<StateMachine>();
        if (machine.isTacklePossible) {
            machine.agent->doCatch();
            return transit<f0>();
        }
        return transit<doMove>();
    }
};

struct doMove : sc::state<doMove, StateMachine> {
    typedef sc::custom_reaction<EvProcess> reactions;
    doMove(my_context ctx) : my_base(ctx) {
        context<StateMachine>().agent->doMove();
    }
    sc::result react(const EvProcess &) {
        return transit<f0>();
    }
};

struct f0 : sc::state<f0, StateMachine> {
    typedef sc::termination reactions;
    f0(my_context ctx) : my_base(ctx) {}
};

int main() {
    rcsc::PlayerAgent agent;
    StateMachine sm;
    sm.agent = &agent;
    sm.initiate();
    sm.process_event(EvProcess());
    return 0;
}