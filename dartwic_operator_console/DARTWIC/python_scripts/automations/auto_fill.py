import dartwic as d

'''
tags the sim uses
|tank_sim/supply_tank_pressure.value| = 1000.0 
|tank_sim/run_tank_pressure.value| = 0.0

|tank_sim/supply_tank_fill.value| = 100.0
|tank_sim/run_tank_fill.value| = 0.0
|tank_sim/run_tank_fill_target.value| = 0.0

|tank_sim/supply_fill_valve.value| = 0.0 
|tank_sim/run_tank_vent_valve.value| = 0.0
'''

#autofill params
|tank_sim/autofill_abort.value| = 0
|tank_sim/autofill_hold.value| = 0

|tank_sim/fill_command.value| = 1

#loop params
|servervariables/loops.autofill.target_loop_frequency.value| = 100.0
#default not running - set to 1 outside of script to run autofill
|servervariables/loops.autofill.running.value| = 0 

@d.on_start("autofill")
def loop():
    #ask test operators to set fill target
    fill_target = d.promptNumerical(
            "Run Tank Auto Fill Target",
            "Autofill sequence has started. Please enter fill target for run tank.",
            ["tank_sim/run_tank_fill_target.value"],
            "Enter target."
        )

    #ensure fill target tag is set
    |tank_sim/run_tank_fill_target.value| = fill_target

@d.on_loop("autofill")
def loop():

    ## HOLDS AND ABORTS
    if |tank_sim/autofill_abort.value| == 1:
        d.abort(
            "RUN TANK AUTO FILL ABORT",
            "An abort has been called on run rank auto fill sequence.",
            ["tank_sim/autofill_abort.value"],
            "Evaluate scene."
        )

        #stop loop
        |servervariables/loops.autofill.running.value| = 0


    if |tank_sim/autofill_hold.value| == 1:

        #safe system
        |tank_sim/supply_fill_valve.value| = 0
        |tank_sim/run_tank_vent_valve.value| = 0

        d.hold(
            "RUN TANK AUTO FILL HOLD",
            "A hold has been called on run rank auto fill sequence.",
            ["tank_sim/autofill_hold.value"],
            "Evaluate and release hold."
        )

        #set hold to 0
        |tank_sim/autofill_hold.value| = 0

     
    ### FILL COMMANDS ; vent = 1, fill = 2
    if |tank_sim/fill_command.value| == 1:

        #close supply fill and vent tank
        |tank_sim/supply_fill_valve.value| = 0
        |tank_sim/run_tank_vent_valve.value| = 1

        #start to fill again after we hit 400 
        if |tank_sim/run_tank_pressure.value| <= 400:
            |tank_sim/fill_command.value| = 2

    if |tank_sim/fill_command.value| == 2:

            #fill tank and close vent
            |tank_sim/supply_fill_valve.value| = 1
            |tank_sim/run_tank_vent_valve.value| = 0


    ### SET NEW FILL COMMAND
    #set fill command
    #pressures equalize - vent until pressure gets to 400
    if |tank_sim/run_tank_pressure.value| >= (|tank_sim/supply_tank_pressure.value|*0.9):
         |tank_sim/fill_command.value| = 1

    ### END OF FILL
    #if fill target has been reached
    if |tank_sim/run_tank_fill.value| >= |tank_sim/run_tank_fill_target.value|:

        #stop loop
        |servervariables/loops.autofill.running.value| = 0

@d.on_end("autofill")
def loop():
    #once fill is complete, close valves
    |tank_sim/supply_fill_valve.value| = 0
    |tank_sim/run_tank_vent_valve.value| = 0

    d.message(
            "Run Tank Auto Fill Complete",
            "Auto fill sequence for the run tank has completed, as run tank fill has reached target level.",
            ["tank_sim/run_tank_fill.value", "tank_sim/run_tank_fill_target.value"],
            "-"
        )