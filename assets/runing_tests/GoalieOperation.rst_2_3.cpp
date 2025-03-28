#include <iostream>
#include <boost/statechart/state_machine.hpp>
#include <boost/statechart/simple_state.hpp>
#include <boost/statechart/custom_reaction.hpp>
#include <boost/statechart/event.hpp>
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
        using reactions = sc::transition<EvCheck, SdoCatch>;
    };

    struct SdoCatch : sc::state<SdoCatch, RoboSimMachine> {
        bool insideGoalieArea;
        using reactions = sc::custom_reaction<EvCheck>;

        SdoCatch(my_context ctx) : my_base(ctx) {
            Vector2D ball_pos = context<RoboSimMachine>().agent->world().ball().pos();
            insideGoalieArea = isInOurPenaltyArea(ball_pos);
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
        using reactions = sc::custom_reaction<EvCheck>;

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
        using reactions = sc::custom_reaction<EvCheck>;

        SdoTackle(my_context ctx) : my_base(ctx) {
            double prob = context<RoboSimMachine>().agent->world().self().tackleProbability();
            isTacklePossible = checkTackle(prob);
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
        using reactions = sc::custom_reaction<EvCheck>;

        DoMove(my_context ctx) : my_base(ctx) {
            const Vector2D& ball = context<RoboSimMachine>().agent->world().ball().pos();
            const Vector2D& goalPos = context<RoboSimMachine>().agent->world().ourGoal().center();
            blockPoint = calculateBlockPoint(ball, goalPos);
        }

        sc::result react(const EvCheck &) {
            context<RoboSimMachine>().agent->doMove(blockPoint);
            return transit<f0>();
        }
    };

    struct f0 : sc::simple_state<f0, RoboSimMachine> {};

    static bool isInOurPenaltyArea(const Vector2D &ball) {
        return ball.absX() < ServerParam::i().ourPenaltyAreaLineX() &&
               ball.absY() < ServerParam::i().penaltyAreaHalfWidth();
    }

    static bool checkTackle(double prob) {
        return prob > 0.5;
    }

    static Vector2D calculateBlockPoint(const Vector2D &ball, const Vector2D &goalPos) {
        Vector2D direction = (goalPos - ball);
        direction.setLength(5.0);
        return ball + direction;
    }

    void execute(PlayerAgent &agent) {
        RoboSimMachine machine;
        machine.agent = &agent;
        machine.initiate();
        machine.process_event(EvCheck());
    }
};