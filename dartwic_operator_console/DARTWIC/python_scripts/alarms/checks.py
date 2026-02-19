import dartwic as d

#loop params
|servervariables/loops.alarm_checks.target_loop_frequency.value| = 10.0
#default not running - set to 1 outside of script to run autofill
|servervariables/loops.alarm_checks.running.value| = 1 

@d.on_loop("alarm_checks")
def loop():

    # put in sensor checks here
    x = 1