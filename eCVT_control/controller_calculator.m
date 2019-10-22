clear;
clc;

% timing
control_period = 20e-3; % [s]

% filter gain
lpf_gain = 1e3;
fprintf('filter gain = %.0e\n', lpf_gain);
% engine rpm
eg_cutoff_freq = 120*2*pi; % [rad/s]
eg_filt_const_1 = eg_cutoff_freq/(2/control_period + eg_cutoff_freq);
eg_filt_const_2 = 1 - 2*eg_filt_const_1;
fprintf('engine:\nA1 = %.0f\nA2 = %.0f\n\n', eg_filt_const_1*lpf_gain, eg_filt_const_2*lpf_gain)

% gearbox rpm
gb_cutoff_freq = 20*2*pi; % [rad/s]
gb_filt_const_1 = gb_cutoff_freq/(2/control_period + gb_cutoff_freq);
gb_filt_const_2 = 1 - 2*gb_filt_const_1;
fprintf('gearbox:\nA1 = %.0f\nA2 = %.0f\n\n', gb_filt_const_1*lpf_gain, gb_filt_const_2*lpf_gain)
