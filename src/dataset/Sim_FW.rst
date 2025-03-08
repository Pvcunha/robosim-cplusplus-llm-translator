interface MovementI{ 
	move(  lv : real , av : real )
	stop()
}

interface ObstacleI{ 
	
	
event obstacle 

} 


controller SimMovement {

	cycleDef cycle ==1
	requires MovementI
	uses ObstacleI
	sref stm_ref0 = SimSMovement
	
	
connection SimMovement on obstacle to stm_ref0 on obstacle

}

stm SimSMovement{
	
	cycleDef   cycle == 1 
	const PI : real, av: real, lv: real
	clock MBC
	
	
	
	input context {
		uses ObstacleI
	}
	
	output context {
		requires MovementI 
	}
	
	initial i0
	junction j0
	junction j1
	
	
	state SMoving {
		entry $ move ( lv , 0 )   
	}

	state DMoving {
		
	}
	
	state STurning {
		entry $ move ( 0 , lv ) 
	}
	
	state DTurning {
		
	}

	transition t0{ 
			from i0 to SMoving
		}
		
	transition  SMoving_to_DMoving {  
			from SMoving to DMoving
		}
		
	transition tid2 {
		from DMoving
		to j1
		action exec
	}	
		

	
	transition tid7 {
		from j1
		to DMoving
		condition not $obstacle
		
	}
	
	transition tid8_toWait {
		from j1
		
	to STurning
		condition 
		$ obstacle
		action  
	# MBC ; $ stop ( ) 
	}
	transition tid3{ 
			from STurning 
			to DTurning
	}
	
	transition tid4 {
		from DTurning
		to j0
		action exec
	}
	
	
	transition tid6 {
		from j0
		to DTurning
		condition 
		 since ( MBC ) < PI / av
	}
	transition tid5 {
		from j0
		to SMoving
		condition since ( MBC ) >= PI / av
	}
	

}



module SimCFootBot {
	
	cycleDef cycle ==1
	
	robotic platform FootBot {
		provides MovementI
		uses ObstacleI
	}
	
	cref ctrl_ref0 = SimMovement 
	connection FootBot on obstacle to ctrl_ref0 on obstacle
( _async )
}

