#include <iostream>
#include <boost/statechart/state_machine.hpp>
#include <boost/statechart/simple_state.hpp>
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

    struct i0;
    struct f0;
    struct SdoCatch;
    struct SClearBall;
    struct SdoTackle;
    struct DoMove;

    struct RoboSimMachine : sc::state_machine< RoboSimMachine, i0 > {
        PlayerAgent *agent;
    };

    struct i0: sc::simple_state< i0, RoboSimMachine > {
        typedef sc::transition< EvCheck, SdoCatch > reactions;
    };

    struct SdoCatch: sc::state< SdoCatch, RoboSimMachine > {
        bool insideGoalieArea;
        typedef sc::custom_reaction< EvCheck > reactions;

        SdoCatch(my_context ctx): my_base(ctx),
        insideGoalieArea(isInOurPenaltyArea(context<RoboSimMachine>().agent->world().ball().pos())) {}

        sc::result react(const EvCheck &) {
            auto &world = context<RoboSimMachine>().agent->world();
            if (world.ball().catchable() && insideGoalieArea) {
                context<RoboSimMachine>().agent->doCatch();
                return transit<f0>();
            }
            return transit<SClearBall>();
        }
    private:
        static bool isInOurPenaltyArea(const Vector2D &ball) {
            return ball.absX() < rcsc::ServerParam::i().ourPenaltyAreaLineX() && ball.absY() < rcsc::ServerParam::i().penaltyAreaHalfWidth();
        }
        bool insideGoalieArea;
    };

    struct SClearBall: sc::simple_state< SClearBall, RoboSimMachine > {
        typedef sc::custom_reaction< EvCheck > reactions;
        sc::result react(const EvCheck &) {
            auto &world = context<RoboSimMachine>().agent->world();
            if (world.ball().isKickable()) {
                context<RoboSimMachine>().agent->doClearBall();
                return transit<f0>();
            }
            return transit<SdoTackle>();
        }
    };

    struct SdoTackle: sc::simple_state< SdoTackle, RoboSimMachine > {
        typedef sc::custom_reaction< EvCheck > reactions;
        SdoTackle() : my_base() {}

        sc::result react(const EvCheck &) {
            double prob = context<RoboSimMachine>().agent->world().self().tackleProbability();
            bool isTacklePossible = prob >= 0.8;
            if (isTacklePossible) {
                context<RoboSimMachine>().agent->doTackle();
                return transit<f0>();
            }
            return transit<doMove>();
        }
    };

    struct DoMove: sc::simple_state< DoMove, RoboSimMachine > {
        Vector2D blockPoint;
        DoMove(my_context ctx): my_base(ctx) {
            auto &world = context<RoboSimMachine>().agent->world();
            blockPoint = calculateBlockPoint(world.ball().pos(), world.ourGoal().center());
            context<RoboSimMachine>().agent->doMove(blockPoint);
        }
        typedef sc::transition< EvCheck, f0 > reactions;

        Vector2D calculateBlockPoint(const Vector2D &ball, const Vector2D &goalPos) {
            Vector2D direction = (goalPos - ball);
            direction.setLength(5.0);
            return ball + direction;
        }
        Vector2D blockPoint;
    };

    struct f0: sc::simple_state< f0, RoboSimMachine > {};

    bool execute(PlayerAgent &agent) {
        RoboSimMachine machine;
        machine.agent = &agent;
        machine.initiate();
        machine.process_event(EvCheck());
        return true;
    }
};