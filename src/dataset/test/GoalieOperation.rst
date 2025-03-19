package operations

operation doGoalie ( agent: server::PlayerAgent ) { 
	var insideGoalieArea : boolean
	var bodyInterceptAct : boolean
	var isTacklePossible : boolean
	var catchable: boolean
	var blockPoint : server::Point
	initial i0
	final f0

	state SdoCatch {
		entry insideGoalieArea = isInOurPenaltyArea ( agent ); catchable = isCatchable(agent)
	}
	
	state SClearBall {
		
	}

	junction j0
	junction j1
	
	junction j2
	state SdoTackle {
		entry isTacklePossible = checkTackle ( agent . worldModel . self . tackleProbability )
	}
	junction j3
	state doMove {
	}
	
	transition t0 {
		from i0
		to SdoCatch
	}
	
	transition t1 {
		from SdoCatch
		to j0
	}
	
	transition t2 {
		from j0
		to j1
	condition catchable /\ insideGoalieArea
		action $ doShoot ( )
	}
	transition t3 {
		from j0
		to SClearBall
		condition not ( catchable /\ insideGoalieArea )
	}
	transition t5 {
		from SClearBall
		to j2
	}
	transition t6 {
		from j2
		to j1
		condition agent . worldModel . self . isKickable
		action $ doTackle ( )
	}
	transition t7 {
		from j2
		to SdoTackle
		condition not agent . worldModel . self . isKickable
	}
	transition t8 {
		from SdoTackle
		to j3
	}
	transition t9 {
		from j3
		to j1
		condition isTacklePossible
		action $ doCatch ( )
	}
	transition t10 {
		from j3
		to doMove
		condition not isTacklePossible
	}
	transition t11 {
		from doMove
		to j1
		action 
	$  doMove ( )
	}
	
	transition t4 {
		from j1
		to f0
	}
	
	input context {  }
	output context { requires server::ShootI requires server::CatchI requires server::TackleI requires server::ClearBallI requires server::MovementI }
}

function isCatchable(agent: server::PlayerAgent) : boolean {}
function checkBody(agent: server::PlayerAgent) : boolean {}
function isInOurPenaltyArea(agent: server::PlayerAgent) : boolean { }
function checkTackle(prob : real) : boolean { }