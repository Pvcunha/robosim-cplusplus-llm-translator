interface MovementI {
    move(lv: real, av: real)
    stop()
}

interface ObstacleI {
    event obstacle
}

controller SimMovement {
    cycleDef cycle == 1
    requires MovementI
    uses ObstacleI
    sref stm_ref0 = SimSMovement

    connection SimMovement on obstacle to stm_ref0 on obstacle
}

stm SimSMovement {
    cycleDef cycle == 1

    var value: real

    input context {
        uses ObstacleI
    }

    output context {
        requires MovementI
    }

    initial i0

    state SSetup {
        entry value = 1
    }

    state SNotEven {
        exit value = value + 1
    }

    state SEven {
    }

    final f0

    transition t0 {
        from i0 to SSetup
    }

    transition t1 {
        from SSetup
        to SNotEven
    }

    transition t2 {
        from SNotEven
        to SNotEven
        condition value % 2 != 0
    }

    transition t3 {
        from SNotEven
        to SEven
        condition value % 2 == 0
    }

    transition t4 {
        from SEven
        to f0
        action $ stop()
    }
}

module SimCFootBot {
    cycleDef cycle == 1

    robotic platform FootBot {
        provides MovementI
        uses ObstacleI
    }

    cref ctrl_ref0 = SimMovement
    connection FootBot on obstacle to ctrl_ref0 on obstacle (_async)
}
