import dartwic as d


### CONTROL MAPPINGS & DEFAULTS ###

## DEFAULTS
|servervariables/loops.dacs_loop.target_loop_frequency.value| = 10

## FINGERS ##
#pointer finger
|hand_controller/relay_1_desired_state.mapped_channel| = "control/pointer_finger.extend.desired_state"
|hand_controller/relay_1_actual_state.mapped_channel| = "control/pointer_finger.extend.actual_state"

|hand_controller/relay_2_desired_state.mapped_channel| = "control/pointer_finger.retract.desired_state"
|hand_controller/relay_2_actual_state.mapped_channel| = "control/pointer_finger.retract.actual_state"

#other fingers
|hand_controller/relay_3_desired_state.mapped_channel| = "control/hand_fingers.extend.desired_state"
|hand_controller/relay_3_actual_state.mapped_channel| = "control/hand_fingers.extend.actual_state"

|hand_controller/relay_4_desired_state.mapped_channel| = "control/hand_fingers.retract.desired_state"
|hand_controller/relay_4_actual_state.mapped_channel| = "control/hand_fingers.retract.actual_state"

## THUMB ##
#rotational actuator
|hand_controller/relay_5_desired_state.mapped_channel| = "control/thumb.extend.desired_state"
|hand_controller/relay_5_actual_state.mapped_channel| = "control/thumb.extend.actual_state"

|hand_controller/relay_6_desired_state.mapped_channel| = "control/thumb.retract.desired_state"
|hand_controller/relay_6_actual_state.mapped_channel| = "control/thumb.retract.actual_state"

#cylinder (opposition = perpendicular to palm, reposition = parallel to palm)
|hand_controller/relay_7_desired_state.mapped_channel| = "control/thumb.opposition.desired_state"
|hand_controller/relay_7_actual_state.mapped_channel| = "control/thumb.opposition.actual_state"

|hand_controller/relay_8_desired_state.mapped_channel| = "control/thumb.reposition.desired_state"
|hand_controller/relay_8_actual_state.mapped_channel| = "control/thumb.reposition.actual_state"


### CUSTOM CONTROL FUNCTIONS ###

# 0 = closed; 1 = open
|controls/open_hand.last_known_state.value| = 0

@d.on_function("toggle_open_hand")
def toggle_open_hand(args):
    #if last known was closed
    if |controls/open_hand.last_known_state.value| == 0:
        |controls/open_hand.last_known_state.value| = 1
        # open hand
        |hand_controller/relay_1_desired_state.value| = 1
        

    #if last known was open
    else:
        |controls/open_hand.last_known_state.value| = 0
        # close hand
        |hand_controller/relay_1_desired_state.value| = 0
