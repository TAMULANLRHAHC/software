import dartwic as d

#define tags needed for sim
max_run_tank_fill = 100
max_run_tank_pressure = 1000

min_run_tank_fill = 0
min_run_tank_pressure = 0

#also a note about the tag language - assignments must be the only thing on the line - so not comments there
|tank_sim/supply_tank_pressure.value| = 1000.0 
|tank_sim/run_tank_pressure.value| = 0.0

|tank_sim/supply_tank_fill.value| = 100.0
|tank_sim/run_tank_fill.value| = 0.0
|tank_sim/run_tank_fill_target.value| = 0.0

|tank_sim/supply_fill_valve.value| = 0.0 
|tank_sim/run_tank_vent_valve.value| = 0.0

#loop
|servervariables/loops.test_sim.target_loop_frequency.value| = 10.0
|servervariables/loops.test_sim.running.value| = 1.0

@d.on_loop("test_sim")
def loop_1():


    #if run tank pressure is near the supply tank pressure, then the run tank cannot fill any more
    if |tank_sim/run_tank_pressure.value| < (|tank_sim/supply_tank_pressure.value|*0.9):

        #run tank controls
        #fill open
        if |tank_sim/supply_fill_valve.value| > 0:
            #increase fill level
            if |tank_sim/run_tank_fill.value| < max_run_tank_fill: 
                |tank_sim/run_tank_fill.value| = |tank_sim/run_tank_fill.value| + 0.2

            #increase run tank pressure
            if |tank_sim/run_tank_pressure.value| < 1000: 
                |tank_sim/run_tank_pressure.value| = |tank_sim/run_tank_pressure.value| + 5


    #vent open
    if |tank_sim/run_tank_vent_valve.value| > 0:
        #decrease fill level
        if |tank_sim/run_tank_fill.value| > min_run_tank_fill:
            |tank_sim/run_tank_fill.value| = |tank_sim/run_tank_fill.value| - 0.1

        #decrease run tank pressure
        if |tank_sim/run_tank_pressure.value| > min_run_tank_pressure:
            |tank_sim/run_tank_pressure.value| = |tank_sim/run_tank_pressure.value| - 5

