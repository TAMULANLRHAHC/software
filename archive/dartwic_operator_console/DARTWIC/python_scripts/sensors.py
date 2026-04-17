import dartwic as d

### AIN VOLTAGE DEFAULTS ###

#for 5V output and 1023 counts
#appears to be an uncertainty of 3% measured value - can add software data processing if needed
adc_scale_10_bit = (5-0) / (1023-0)
adc_offset = 0

#for 0-500 PSIG & 0.5-4.5V pressure transducers
pt_scale = (500-0) / (4.5-0.5)
#offset exists as there is a lower bound to the voltage, ie, 0.5.
pt_offset = -0.5*pt_scale

#raw ADC value to PT
adc_to_pt_scale = pt_scale * adc_scale_10_bit
adc_to_pt_offset = pt_offset + adc_offset

### SENSOR MAPPINGS ###

#hpu
|hand_controller/AIN0_raw.mapped_channel| = "sensors/HPU.pressure"
|sensors/HPU.pressure.scale| = adc_to_pt_scale
|sensors/HPU.pressure.offset| = adc_to_pt_offset

#finger cylinders
|hand_controller/AIN1_raw.mapped_channel| = "sensors/HPU.pressure"
|sensors/HPU.pressure.scale| = adc_to_pt_scale
|sensors/HPU.pressure.offset| = adc_to_pt_offset

|hand_controller/AIN2_raw.mapped_channel| = "sensors/hand_fingers.extension_cylinder.pressure"
|sensors/hand_fingers.extension_cylinder.pressure.scale| = adc_to_pt_scale
|sensors/hand_fingers.extension_cylinder.pressure.offset| = adc_to_pt_offset

#thumb cylinders & actuators
|hand_controller/AIN3_raw.mapped_channel| = "sensors/thumb.extend_rotational_actuator.pressure"
|sensors/thumb.extend_rotational_actuator.pressure.scale| = adc_to_pt_scale
|sensors/thumb.extend_rotational_actuator.pressure.offset| = adc_to_pt_offset

|hand_controller/AIN4_raw.mapped_channel| = "sensors/thumb.opposition_cylinder.pressure"
|sensors/thumb.opposition_cylinder.pressure.scale| = adc_to_pt_scale
|sensors/thumb.opposition_cylinder.pressure.offset| = adc_to_pt_offset
