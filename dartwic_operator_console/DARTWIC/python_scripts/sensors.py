import dartwic as d

### SENSOR MAPPINGS & DEFAULTS ###

#hpu
|hand_controller/AIN0_raw.mapped_channel| = "sensors/HPU.pressure"
|hand_controller/AIN0_raw.scale| = 1
|hand_controller/AIN0_raw.offset| = 0

#finger cylinders
|hand_controller/AIN1_raw.mapped_channel| = "sensors/pointer_finger.extension_cylinder.pressure"
|hand_controller/AIN1_raw.scale| = 1
|hand_controller/AIN1_raw.offset| = 0

|hand_controller/AIN2_raw.mapped_channel| = "sensors/hand_fingers.extension_cylinder.pressure"
|hand_controller/AIN2_raw.scale| = 1
|hand_controller/AIN2_raw.offset| = 0

#thumb cylinders & actuators
|hand_controller/AIN3_raw.mapped_channel| = "sensors/thumb.extend_rotational_actuator.pressure"
|hand_controller/AIN3_raw.scale| = 1
|hand_controller/AIN3_raw.offset| = 0

|hand_controller/AIN4_raw.mapped_channel| = "sensors/thumb.opposition_cylinder.pressure"
|hand_controller/AIN4_raw.scale| = 1
|hand_controller/AIN4_raw.offset| = 0
