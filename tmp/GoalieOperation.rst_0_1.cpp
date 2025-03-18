
#pragma once
#include <boost/statechart/state_machine.hpp>
#include <boost/statechart/simple_state.hpp>
#include <boost/statechart/transition.hpp>
#include <boost/statechart/custom_reaction.hpp>
#include "rcsc/player/player_agent.h"
#include "rcsc/common/server_param.h"
#include "rcsc/geom/vector_2d.h"
#include "rcsc/player/action_effector.h"

namespace sc = boost::statechart;
namespace mpl = boost::mpl;

// Forward declarations of states and events
struct SdoCatch; struct SClearBall; struct SdoTackle; struct doMove; struct FinalState;
struct GoalkeeperFSM;

// Events
enum class Event { Evaluate };

// State machine definition
struct GoalkeeperFSM : sc::state_machine<GoalkeeperFSM, SdoCatch> {
    rcsc::PlayerAgent &agent;
    bool insideGoalieArea;
    bool isTacklePossible;
    rcsc::Vector2D blockPoint;
    explicit GoalkeeperFSM(rcsc::PlayerAgent &a) : agent(a) {}
};

// States
struct SdoCatch : sc::simple_state<SdoCatch, GoalkeeperFSM> {
    using reactions = mpl::list<sc::transition<Event, SClearBall>, sc::transition<Event, FinalState>>;
    SdoCatch() {
        context<GoalkeeperFSM>().insideGoalieArea = isInOurPenaltyArea(context<GoalkeeperFSM>().agent.world().ball().pos());
        if (context<GoalkeeperFSM>().agent.world().ball().catchable() && context<GoalkeeperFSM>().insideGoalieArea) {
            context<GoalkeeperFSM>().agent.doCatch();
            post_event(Event::Evaluate);
        } else {
            post_event(Event::Evaluate);
        }
    }
};

struct SClearBall : sc::simple_state<SClearBall, GoalkeeperFSM> {
    using reactions = mpl::list<sc::transition<Event, SdoTackle>>;
    SClearBall() {
        if (context<GoalkeeperFSM>().agent.world().ball().isKickable()) {
            context<GoalkeeperFSM>().agent.doKick(100, context<GoalkeeperFSM>().agent.world().ball().angleFromSelf());
            post_event(Event::Evaluate);
        } else {
            post_event(Event::Evaluate);
        }
    }
};

struct SdoTackle : sc::simple_state<SdoTackle, GoalkeeperFSM> {
    using reactions = mpl::list<sc::transition<Event, doMove>, sc::transition<Event, FinalState>>;
    SdoTackle() {
        context<GoalkeeperFSM>().isTacklePossible = checkTackle(context<GoalkeeperFSM>().agent.world().self().tackleProbability());
        if (context<GoalkeeperFSM>().isTacklePossible) {
            context<GoalkeeperFSM>().agent.doTackle();
            post_event(Event::Evaluate);
        } else {
            post_event(Event::Evaluate);
        }
    }
};

struct doMove : sc::simple_state<doMove, GoalkeeperFSM> {
    using reactions = sc::transition<Event, FinalState>;
    doMove() {
        context<GoalkeeperFSM>().blockPoint = calculateBlockPoint(context<GoalkeeperFSM>().agent.world().ball().pos(), context<GoalkeeperFSM>().agent.world().ourGoal().center());
        context<GoalkeeperFSM>().agent.doMove(context<GoalkeeperFSM>().blockPoint.x, context<GoalkeeperFSM>().blockPoint.y);
        post_event(Event::Evaluate);
    }
};

struct FinalState : sc::simple_state<FinalState, GoalkeeperFSM> {};

// Utility Functions
bool isInOurPenaltyArea(const rcsc::Vector2D& ball) {
    return ball.x < -rcsc::ServerParam::i().pitchHalfLength() + rcsc::ServerParam::i().penaltyAreaLength()
           && std::abs(ball.y) < rcsc::ServerParam::i().penaltyAreaWidth() / 2.0;
}

bool checkTackle(double probability) {
    return probability > 0.5; // Example threshold
}

rcsc::Vector2D calculateBlockPoint(const rcsc::Vector2D& ball, const rcsc::Vector2D& goalPos) {
    return goalPos + (ball - goalPos).setLength(1.0); // Example logic
}
