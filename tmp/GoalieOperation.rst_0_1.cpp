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

class GoalieBehavior {
public:
    struct EvProcess : sc::event<EvProcess> {};
    struct i0;
    struct f0;
    struct SdoCatch;
    struct SClearBall;
    struct SdoTackle;
    struct doMove;
    struct GoalieStateMachine : sc::state_machine<GoalieStateMachine, i0> {};

    struct i0 : sc::state<i0, GoalieStateMachine> {
        typedef sc::custom_reaction<EvProcess> reactions;
        i0(my_context ctx) : my_base(ctx) {}
        sc::result react(const EvProcess&) {
            return transit<SdoCatch>();
        }
    };

    struct SdoCatch : sc::state<SdoCatch, GoalieStateMachine> {
        typedef sc::custom_reaction<EvProcess> reactions;
        SdoCatch(my_context ctx) : my_base(ctx) {
            insideGoalieArea = isInOurPenaltyArea(context<GoalieStateMachine>().agent->world().ball());
        }
        sc::result react(const EvProcess&) {
            if (context<GoalieStateMachine>().agent->world().catchable() && insideGoalieArea) {
                doCatch();
                return transit<f0>();
            }
            return transit<SClearBall>();
        }
    };

    struct SClearBall : sc::state<SClearBall, GoalieStateMachine> {
        typedef sc::custom_reaction<EvProcess> reactions;
        SClearBall(my_context ctx) : my_base(ctx) {}
        sc::result react(const EvProcess&) {
            if (context<GoalieStateMachine>().agent->world().isKickable()) {
                doClearBall();
                return transit<f0>();
            }
            return transit<SdoTackle>();
        }
    };

    struct SdoTackle : sc::state<SdoTackle, GoalieStateMachine> {
        typedef sc::custom_reaction<EvProcess> reactions;
        SdoTackle(my_context ctx) : my_base(ctx) {
            isTacklePossible = checkTackle(context<GoalieStateMachine>().agent->world().tackleProbability());
        }
        sc::result react(const EvProcess&) {
            if (isTacklePossible) {
                doTackle();
                return transit<f0>();
            }
            return transit<doMove>();
        }
    };

    struct doMove : sc::state<doMove, GoalieStateMachine> {
        typedef sc::custom_reaction<EvProcess> reactions;
        doMove(my_context ctx) : my_base(ctx) {
            blockPoint = calculateBlockPoint(context<GoalieStateMachine>().agent->world().ball(), context<GoalieStateMachine>().agent->world().goalPosition());
        }
        sc::result react(const EvProcess&) {
            doMove(blockPoint);
            return transit<f0>();
        }
    };

    struct f0 : sc::state<f0, GoalieStateMachine> {
        typedef sc::custom_reaction<EvProcess> reactions;
        f0(my_context ctx) : my_base(ctx) {}
        sc::result react(const EvProcess&) {
            return terminate();
        }
    };

    bool execute(rcsc::PlayerAgent* agent) {
        GoalieStateMachine machine;
        machine.initiate();
        while (!machine.terminated()) {
            machine.process_event(EvProcess());
        }
        return true;
    }

private:
    bool insideGoalieArea;
    bool isTacklePossible;
    rcsc::Vector2D blockPoint;

    static bool isInOurPenaltyArea(const rcsc::Vector2D& ball) { /* Implement logic */ }
    bool checkTackle(double prob) { /* Implement logic */ }
    rcsc::Vector2D calculateBlockPoint(const rcsc::Vector2D& ball, const rcsc::Vector2D& goalPos) { /* Implement logic */ }
    void doCatch() { /* Implement logic */ }
    void doClearBall() { /* Implement logic */ }
    void doTackle() { /* Implement logic */ }
    void doMove(const rcsc::Vector2D& point) { /* Implement logic */ }
};

int main() {
    rcsc::PlayerAgent agent;
    GoalieBehavior goalie;
    goalie.execute(&agent);
    return 0;
}
