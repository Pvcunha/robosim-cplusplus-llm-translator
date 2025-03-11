#include <iostream>
#include <boost/statechart/state_machine.hpp>
#include <boost/statechart/simple_state.hpp>
#include <boost/statechart/event.hpp>
#include <boost/statechart/custom_reaction.hpp>
#include <rcsc/player/player_agent.h>
#include <rcsc/common/server_param.h>
#include <rcsc/geom/vector_2d.h>

namespace sc = boost::statechart;
using namespace rcsc;

class StateMachine {
public:
    struct EvCheck : sc::event<EvCheck> {};

    struct i0; struct f0; struct SdoCatch; struct SClearBall; struct SdoTackle; struct DoMove;

    struct RoboSimMachine : sc::state_machine<RoboSimMachine, i0> {
        PlayerAgent* agent;
    };

    struct i0 : sc::simple_state<i0, RoboSimMachine> {
        typedef sc::transition<EvCheck, SdoCatch> reactions;
    };

    struct SdoCatch : sc::state<SdoCatch, RoboSimMachine> {
        bool insideGoalieArea;
        typedef sc::custom_reaction<EvCheck> reactions;

        SdoCatch(my_context ctx) : my_base(ctx) {
            insideGoalieArea = isInOurPenaltyArea(context<RoboSimMachine>().agent->world().ball().pos());
        }

        sc::result react(const EvCheck &) {
            if (context<RoboSimMachine>().agent->world().ball().catchable() && insideGoalieArea) {
                context<RoboSimMachine>().agent->doCatch();
                return transit<f0>();
            }
            return transit<SClearBall>();
        }
    };

    struct SClearBall : sc::state<SClearBall, RoboSimMachine> {
        typedef sc::custom_reaction<EvCheck> reactions;

        SClearBall(my_context ctx) : my_base(ctx) {}

        sc::result react(const EvCheck &) {
            if (context<RoboSimMachine>().agent->world().ball().isKickable()) {
                context<RoboSimMachine>().agent->doClearBall();
                return transit<f0>();
            }
            return transit<SdoTackle>();
        }
    };

    struct SdoTackle : sc::state<SdoTackle, RoboSimMachine> {
        bool isTacklePossible;
        typedef sc::custom_reaction<EvCheck> reactions;

        SdoTackle(my_context ctx) : my_base(ctx) {
            isTacklePossible = checkTackle(context<RoboSimMachine>().agent->world().self().tackleProbability());
        }

        sc::result react(const EvCheck &) {
            if (isTacklePossible) {
                context<RoboSimMachine>().agent->doTackle();
                return transit<f0>();
            }
            return transit<DoMove>();
        }
    };

    struct DoMove : sc::state<DoMove, RoboSimMachine> {
        Vector2D blockPoint;
        typedef sc::custom_reaction<EvCheck> reactions;

        DoMove(my_context ctx) : my_base(ctx) {
            blockPoint = calculateBlockPoint(context<RoboSimMachine>().agent->world().ball().pos(), context<RoboSimMachine>().agent->world().ourGoal().center());
        }

        sc::result react(const EvCheck &) {
            context<RoboSimMachine>().agent->doMove(blockPoint);
            return transit<f0>();
        }
    };

    struct f0 : sc::simple_state<f0, RoboSimMachine> {};

    static bool isInOurPenaltyArea(const Vector2D &ball) {
        return ball.absX() < ServerParam::i().ourPenaltyAreaLineX() && ball.absY() < ServerParam::i().penaltyAreaHalfWidth();
    }

    static bool checkTackle(double prob) {
        return prob > 0.5;
    }

    static Vector2D calculateBlockPoint(const Vector2D &ball, const Vector2D &goalPos) {
        Vector2D direction = (goalPos - ball).normalized();
        return ball + direction.setLength(5.0);
    }

    void execute(PlayerAgent &agent) {
        RoboSimMachine machine;
        machine.agent = &agent;
        machine.initiate();
        machine.process_event(EvCheck());
    }
};