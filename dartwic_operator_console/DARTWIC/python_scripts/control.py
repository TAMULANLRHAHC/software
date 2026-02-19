import dartwic as d

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
