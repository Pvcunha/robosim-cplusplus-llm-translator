#include <iostream>
#include <boost/statechart/state_machine.hpp>
#include <boost/statechart/simple_state.hpp>
#include <boost/statechart/transition.hpp>
#include <boost/statechart/state.hpp>
#include <boost/statechart/event.hpp>
#include <boost/statechart/custom_reaction.hpp>
#include <rcsc/player/player_agent.h>
#include <rcsc/common/server_param.h>
#include <rcsc/geom/vector_2d.h>

namespace sc = boost::statechart;
using rcsc::PlayerAgent;
using rcsc::Vector2D;

class StateMachine {
public:
    struct EvCheck : sc::event< EvCheck > {};

    struct i0; struct f0; struct SdoCatch; struct SClearBall; struct SdoTackle; struct DoMove;

    struct RoboSimMachine : sc::state_machine< RoboSimMachine, i0 > {
        PlayerAgent *agent;
    };

    struct i0: sc::state< i0, RoboSimMachine > {
        typedef sc::transition< EvCheck, SdoCatch > reactions;
        i0(my_context ctx): my_base(ctx) {}
    };

    struct SdoCatch: sc::state< SdoCatch, RoboSimMachine > {
        bool insideGoalieArea;
        typedef sc::custom_reaction< EvCheck > reactions;

        SdoCatch(my_context ctx) : my_base(ctx) {
            auto ball_pos = context<RoboSimMachine>().agent->world().ball().pos();
            insideGoalieArea = isInOurPenaltyArea(ball_pos);
        }

        sc::result react(const EvCheck &) {
            auto &world = context<RoboSimMachine>().agent->world();
            if (world.ball().catchable() && insideGoalieArea) {
                context<RoboSimMachine>().agent->doCatch();
                return transit<f0>();
            }
            return transit<SClearBall>();
        }

        bool insideGoalieArea;
        static bool isInOurPenaltyArea(const rcsc::Vector2D &ball) {
            return ball.absX() < rcsc::ServerParam::i().ourPenaltyAreaLineX()
                && ball.absY() < rcsc::ServerParam::i().penaltyAreaHalfWidth();
        }
    };

    struct SClearBall: sc::state< SClearBall, RoboSimMachine > {
        typedef sc::custom_reaction< EvCheck > reactions;

        SClearBall(my_context ctx): my_base(ctx) {}

        sc::result react(const EvCheck &) {
            if (context<RoboSimMachine>().agent->world().ball().isKickable()) {
                context<RoboSimMachine>().agent->doClearBall();
                return transit<f0>();
            }
            return transit<SdoTackle>();
        }
    };

    struct SdoTackle: sc::state< SdoTackle, RoboSimMachine > {
        bool isTacklePossible;

        SdoTackle(my_context ctx): my_base(ctx) {
            double prob = context<RoboSimMachine>().agent->world().self().tackleProbability();
            isTacklePossible = checkTackle(prob);
        }

        sc::result react(const EvCheck &) {
            if (isTacklePossible) {
                context<RoboSimMachine>().agent->doTackle();
                return transit<f0>();
            }
            return transit<doMove>();
        }

        static bool checkTackle(double prob) {
            return prob > 0.8;
        }
    };

    struct doMove : sc::state< doMove, RoboSimMachine > {
        Vector2D blockPoint;

        doMove(my_context ctx) : my_base(ctx) {
            auto ball = context<RoboSimMachine>().agent->world().ball().pos();
            auto goalPos = context<RoboSimMachine>().agent->world().ourGoal().center();
            blockPoint = calculateBlockPoint(ball, goalPos);
            context<RoboSimMachine>().agent->doMove(blockPoint);
        }

        typedef sc::transition<EvCheck, f0> reactions;

        static Vector2D calculateBlockPoint(const Vector2D &ball, const Vector2D &goalPos) {
            Vector2D direction = (goalPos - ball).normalized();
            return ball + direction * 5.0;
        }
    };

    struct f0 : sc::simple_state< f0, RoboSimMachine > {};

    bool execute(PlayerAgent &agent) {
        RoboSimMachine machine;
        machine.agent = &agent;
        machine.initiate();
        machine.process_event(EvCheck());
        return true;
    }
};