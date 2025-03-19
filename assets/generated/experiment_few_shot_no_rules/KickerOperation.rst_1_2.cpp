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

bool isKickable();
bool canShootToGoal();

class StateMachine {
public:
    struct EvAction : sc::event<EvAction> {};

    struct i0;
    struct f0;
    struct SGoToBall;
    struct SShoot;
    struct SDribble;

    struct Machine : sc::state_machine<Machine, i0> {
        rcsc::PlayerAgent* agent;
        bool canShoot;
        bool isKickable;

        Machine(rcsc::PlayerAgent* agent) : agent(agent), canShoot(false), isKickable(false) {}
    };

    struct i0 : sc::state<i0, Machine> {
        typedef sc::transition<EvAction, SGoToBall> reactions;
        i0(my_context ctx) : my_base(ctx) {}
    };

    struct SGoToBall : sc::state<SGoToBall, Machine> {
        typedef sc::transition<EvAction, SShoot> reactions;
        SGoToBall(my_context ctx) : my_base(ctx) {
            context<Machine>().isKickable = isKickable();
            if (!context<Machine>().isKickable) {
                context<Machine>().agent->doMove(context<Machine>().agent->world().ball().pos());
            }
        }
    };

    struct SShoot : sc::state<SShoot, Machine> {
        typedef sc::custom_reaction<EvAction> reactions;
        SShoot(my_context ctx) : my_base(ctx) {
            context<Machine>().canShoot = canShootToGoal();
        }

        sc::result react(const EvAction&) {
            if (context<Machine>().canShoot) {
                context<Machine>().agent->doShoot();
                return transit<f0>();
            } else {
                return transit<SDribble>();
            }
        }
    };

    struct SDribble : sc::state<SDribble, Machine> {
        typedef sc::transition<EvAction, f0> reactions;
        SDribble(my_context ctx) : my_base(ctx) {
            context<Machine>().agent->doDribble();
        }
    };

    struct f0 : sc::termination<f0> {};

    void execute(rcsc::PlayerAgent* agent) {
        Machine machine(agent);
        machine.initiate();
        while (!machine.terminated()) {
            machine.process_event(EvAction());
        }
    }
};