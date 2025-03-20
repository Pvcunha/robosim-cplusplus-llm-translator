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

class GoalieBehavior : public rcsc::SoccerBehavior {
public:
    struct EvTransition : sc::event<EvTransition> {};

    struct i0;
    struct f0;
    struct SdoCatch;
    struct SClearBall;
    struct SdoTackle;
    struct DoMove;

    struct GoalieStateMachine : sc::state_machine<GoalieStateMachine, i0> {
        rcsc::PlayerAgent* agent;
        bool insideGoalieArea;
        bool catchable;
        bool isTacklePossible;

        GoalieStateMachine(rcsc::PlayerAgent* ag) : agent(ag) {}
    };

    struct i0 : sc::state<i0, GoalieStateMachine> {
        typedef sc::custom_reaction<EvTransition> reactions;
        i0(my_context ctx) : my_base(ctx) {}

        sc::result react(const EvTransition&) {
            return transit<SdoCatch>();
        }
    };

    struct SdoCatch : sc::state<SdoCatch, GoalieStateMachine> {
        typedef sc::custom_reaction<EvTransition> reactions;
        SdoCatch(my_context ctx) : my_base(ctx) {
            context<GoalieStateMachine>().insideGoalieArea = isInOurPenaltyArea(context<GoalieStateMachine>().agent);
            context<GoalieStateMachine>().catchable = isCatchable(context<GoalieStateMachine>().agent);
        }

        sc::result react(const EvTransition&) {
            if (context<GoalieStateMachine>().catchable && context<GoalieStateMachine>().insideGoalieArea) {
                context<GoalieStateMachine>().agent->doShoot();
                return transit<f0>();
            }
            return transit<SClearBall>();
        }
    };

    struct SClearBall : sc::state<SClearBall, GoalieStateMachine> {
        typedef sc::custom_reaction<EvTransition> reactions;
        SClearBall(my_context ctx) : my_base(ctx) {}

        sc::result react(const EvTransition&) {
            if (context<GoalieStateMachine>().agent->world().self().isKickable()) {
                context<GoalieStateMachine>().agent->doTackle();
                return transit<f0>();
            }
            return transit<SdoTackle>();
        }
    };

    struct SdoTackle : sc::state<SdoTackle, GoalieStateMachine> {
        typedef sc::custom_reaction<EvTransition> reactions;
        SdoTackle(my_context ctx) : my_base(ctx) {
            context<GoalieStateMachine>().isTacklePossible = checkTackle(context<GoalieStateMachine>().agent->world().self().tackleProbability());
        }

        sc::result react(const EvTransition&) {
            if (context<GoalieStateMachine>().isTacklePossible) {
                context<GoalieStateMachine>().agent->doCatch();
                return transit<f0>();
            }
            return transit<doMove>();
        }
    };

    struct doMove : sc::state<doMove, GoalieStateMachine> {
        typedef sc::custom_reaction<EvTransition> reactions;
        doMove(my_context ctx) : my_base(ctx) {}

        sc::result react(const EvTransition&) {
            context<GoalieStateMachine>().agent->doMove();
            return transit<f0>();
        }
    };

    struct f0 : sc::termination<f0> {};

    bool execute(rcsc::PlayerAgent* agent) {
        GoalieStateMachine machine(agent);
        machine.initiate();
        machine.process_event(EvTransition());
        return true;
    }

    bool isCatchable(rcsc::PlayerAgent* agent);
    bool isInOurPenaltyArea(rcsc::PlayerAgent* agent);
    bool checkTackle(double prob);
};