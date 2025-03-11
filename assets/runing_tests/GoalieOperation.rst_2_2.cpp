#include <iostream>
#include <boost/statechart/state_machine.hpp>
#include <boost/statechart/simple_state.hpp>
#include <boost/statechart/custom_reaction.hpp>
#include <boost/statechart/transition.hpp>
#include <rcsc/player/player_agent.h>
#include <rcsc/common/server_param.h>
#include <rcsc/geom/vector_2d.h>

namespace sc = boost::statechart;
using namespace rcsc;

struct EvCheck : sc::event<EvCheck> {};

class StateMachine {
public:
  struct i0;
  struct f0;
  struct SdoCatch;
  struct SClearBall;
  struct SdoTackle;
  struct DoMove;

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
    DoMove(my_context ctx) : my_base(ctx) {
      auto &world = context<RoboSimMachine>().agent->world();
      blockPoint = calculateBlockPoint(world.ball().pos(), world.ourGoal().center());
      context<RoboSimMachine>().agent->doMove(blockPoint);
    }
    using reactions = sc::transition<EvCheck, f0>;
  };

  struct f0 : sc::state<f0, RoboSimMachine> {
    f0(my_context ctx) : my_base(ctx) {}
    using reactions = sc::custom_reaction<EvCheck>;

    sc::result react(const EvCheck &) {
      return terminate();
    }
  };

  static bool isInOurPenaltyArea(const Vector2D &ball) {
    return ball.absX() < ServerParam::i().ourPenaltyAreaLineX()
        && ball.absY() < ServerParam::i().penaltyAreaHalfWidth();
  }

  static bool checkTackle(double prob) {
    return prob > 0.5;
  }

  static Vector2D calculateBlockPoint(const Vector2D &ball, const Vector2D &goalPos) {
    Vector2D direction = (goalPos - ball).normalized();
    return ball + direction * 5.0;
  }

  bool execute(PlayerAgent &agent) {
    RoboSimMachine machine;
    machine.agent = &agent;
    machine.initiate();
    machine.process_event(EvCheck());
    return true;
  }
};