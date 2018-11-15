create_clock -period 20 -waveform {0 6} -name clk clk
set_input_delay -clock clk 0 [all_inputs]
set_output_delay -clock clk 90 [all_outputs]
set_input_transition -clock clk 33 [all_inputs]
set_load  35   [all_outputs]
