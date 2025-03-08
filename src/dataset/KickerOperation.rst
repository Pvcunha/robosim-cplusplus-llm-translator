package operations

operation doKicker ( agent : server::PlayerAgent ) {
	var canShoot : boolean
	var isKickable : boolean
	initial i0
	final f0

	state SGoToBall {
		entry isKickable = isKickable()
	}

	junction j0
	state SShoot {
		entry canShoot = canShootToGoal ( )
	}

	state SDribble {
		entry $ doDribble ( )
	}

	junction j2

	junction j1

	transition t4 {
		from j0
		to SShoot
	condition isKickable
	}

	transition t1 {
		from SGoToBall
		to j0
	}
	transition t2 {
		from j2
		to SDribble
		condition not canShoot
	}
	transition t5 {
		from SShoot
		to j2
	}

	/*transition t3 {
		from j0
		to j1
		condition

		not isKickable
		action $ doMove ( agent . worldModel . ball )
	}
	transition t6 {
		from j2
		to j1
		condition canShoot
		action $ doShoot ( )
	}*/
transition t0 {
		from i0
		to SGoToBall
	}
	transition t3 {
		from j1
		to f0
	}
	transition t6 {
		from j0
		to j1
		condition not isKickable
		action $ doMove ( agent . worldModel . ball )
	}
	transition t7 {
		from SShoot
		to j1
		condition canShoot
		action $ doShoot ( )
	}
	transition t8 {
		from SDribble
		to j1
	}
	input context {  }
	output context { requires server::ShootI requires server::MovementI }
}

function isKickable() : boolean {}

function canShootToGoal(): boolean { }
