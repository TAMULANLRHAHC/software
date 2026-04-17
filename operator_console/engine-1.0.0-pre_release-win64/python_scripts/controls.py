# channels used

|control/pointer_finger_extend.value| = 0
|control/pointer_finger_extend_state.value| = 0

# |control/pointer_finger_retract.value| = 0
# |control/pointer_finger_retract_state.value| = 0

|control/hand_fingers_extend.value| = 0
|control/hand_fingers_extend_state.value| = 0

# |control/hand_fingers_retract.value| = 0
# |control/hand_fingers_retract_state.value| = 0

|control/thumb_extend.value| = 0
|control/thumb_extend_state.value| = 0

# |control/thumb_retract.value| = 0
# |control/thumb_retract_state.value| = 0

|control/thumb_opposition.value| = 0
|control/thumb_opposition_state.value| = 0

# |control/thumb_reposition.value| = 0
# |control/thumb_reposition_state.value| = 0


# control

@on_task_start("hand", "control_automation")
def start():
    pass



@on_task("hand", "control_automation")
def loop(elapsed_seconds):

    # if not connected - apply no control automaitons
    if |hand/info.connected.value| == 0:
        return
    
    # pointer finger
    if |control/pointer_finger_extend.value| == 1:
        |hand/relay_1.value| = 1
        |hand/relay_2.value| = 0

    else:
        |hand/relay_1.value| = 0
        |hand/relay_2.value| = 1
    
    if (|hand/relay_1_state.value| == 1 and |hand/relay_2_state.value| == 0 ):
        |control/pointer_finger_extend_state.value| = 1
    else:
        |control/pointer_finger_extend_state.value| = 0

    
    # hand fingers
    if |control/hand_fingers_extend.value| == 1:
        |hand/relay_3.value| = 1
        |hand/relay_4.value| = 0

    else:
        |hand/relay_3.value| = 0
        |hand/relay_4.value| = 1
    
    if (|hand/relay_3_state.value| == 1 and |hand/relay_4_state.value| == 0 ):
        |control/hand_fingers_extend_state.value| = 1
    else:
        |control/hand_fingers_extend_state.value| = 0

    # thumb extend / retract
    if |control/thumb_extend.value| == 1:
        |hand/relay_5.value| = 1
        |hand/relay_6.value| = 0

    else:
        |hand/relay_5.value| = 0
        |hand/relay_6.value| = 1
    
    if (|hand/relay_5_state.value| == 1 and |hand/relay_6_state.value| == 0 ):
        |control/thumb_extend_state.value| = 1
    else:
        |control/thumb_extend_state.value| = 0

    # thumb oposition / reposition
    if |control/thumb_opposition.value| == 1:
        |hand/relay_7.value| = 1
        |hand/relay_8.value| = 0

    else:
        |hand/relay_7.value| = 0
        |hand/relay_8.value| = 1
    
    if (|hand/relay_7_state.value| == 1 and |hand/relay_8_state.value| == 0 ):
        |control/thumb_opposition_state.value| = 1
    else:
        |control/thumb_opposition_state.value| = 0


@on_task_end("hand", "control_automation")
def end():
    # turn off all relays
    |hand/relay_1.value| = 0
    |hand/relay_2.value| = 0
    |hand/relay_3.value| = 0
    |hand/relay_4.value| = 0
    |hand/relay_5.value| = 0
    |hand/relay_6.value| = 0
    |hand/relay_7.value| = 0
    |hand/relay_8.value| = 0


