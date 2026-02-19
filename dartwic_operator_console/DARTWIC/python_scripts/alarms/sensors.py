import dartwic as d

#loop params
|servervariables/loops.sensor_checks.target_loop_frequency.value| = 10.0
#default not running - set to 1 outside of script to run autofill
|servervariables/loops.sensor_checks.running.value| = 1 

@d.on_loop("sensor_checks")
def loop():

    # when pressures equalize
    if |tank_sim/run_tank_pressure.value| >= (|tank_sim/supply_tank_pressure.value|*0.9):
        d.warning(
            "Run Tank Pressure Warning",
            "The run tank has reached around the same pressure as the supply tank, reducing the ability to fill efficiently.",
            ["tank_sim/run_tank_pressure.value",
                "tank_sim/supply_tank_pressure.value"
            ],
            "Vent the run tank slightly to remove some pressure.",
            5
        )