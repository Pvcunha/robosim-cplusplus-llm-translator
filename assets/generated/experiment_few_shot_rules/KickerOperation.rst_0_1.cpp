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

class StateMachine : public rcsc::SoccerBehavior {
public:
    struct EvTransition : sc::event<EvTransition> {};

    struct i0;
    struct f0;
    struct SGoToBall;
    struct SShoot;
    struct SDribble;

    struct StateMachineSM : sc::state_machine<StateMachineSM, i0> {
        bool canShoot;
        bool isKickable;
        rcsc::PlayerAgent *agent;

        StateMachineSM(rcsc::PlayerAgent *agent) : agent(agent) {}
    };

    struct i0 : sc::state<i0, StateMachineSM> {
        typedef sc::custom_reaction<EvTransition> reactions;
        i0(my_context ctx) : my_base(ctx) {}

        sc::result react(const EvTransition &) {
            return transit<SGoToBall>();
        }
    };

    struct SGoToBall : sc::state<SGoToBall, StateMachineSM> {
        typedef sc::custom_reaction<EvTransition> reactions;
        SGoToBall(my_context ctx) : my_base(ctx) {
            context<StateMachineSM>().isKickable = isKickable();
        }

        sc::result react(const EvTransition &) {
            return transit<SShoot>();
        }
    };

    struct SShoot : sc::state<SShoot, StateMachineSM> {
        typedef sc::custom_reaction<EvTransition> reactions;
        SShoot(my_context ctx) : my_base(ctx) {
            context<StateMachineSM>().canShoot = canShootToGoal();
        }

        sc::result react(const EvTransition &) {
            if (context<StateMachineSM>().canShoot) {
                doShoot();
                return transit<f0>();
            } else {
                return transit<SDribble>();
            }
        }
    };

    struct SDribble : sc::state<SDribble, StateMachineSM> {
        typedef sc::custom_reaction<EvTransition> reactions;
        SDribble(my_context ctx) : my_base(ctx) {
            doDribble();
        }

        sc::result react(const EvTransition &) {
            return transit<f0>();
        }
    };

    struct f0 : sc::state<f0, StateMachineSM> {
        f0(my_context ctx) : my_base(ctx) {}
    };

    static bool isKickable() {
        // Implementation for checking kickable condition
        return true;
    }

    static bool canShootToGoal() {
        // Implementation for checking shoot condition
        return true;
    }

    static void doShoot() {
        // Implementation for shooting action
    }

    static void doDribble() {
        // Implementation for dribbling action
    }

    bool execute(rcsc::PlayerAgent *agent) {
        StateMachineSM sm(agent);
        sm.initiate();
        while (!sm.terminated()) {
            sm.process_event(EvTransition());
        }
        return true;
    }
};