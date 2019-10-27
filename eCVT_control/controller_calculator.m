%% 
clear;
clc;

% timing
control_period = 40e-3; % [s]

% filter gain
lpf_gain = 1;
fprintf('filter gain = %.0e\n', lpf_gain);
% engine rpm
eg_cutoff_freq = 10*2*pi; % [rad/s]
eg_filt_const_1 = eg_cutoff_freq/(2/control_period + eg_cutoff_freq);
eg_filt_const_2 = 1 - 2*eg_filt_const_1;
fprintf('engine:\nA1 = %.3f\nA2 = %.3f\n\n', eg_filt_const_1*lpf_gain, eg_filt_const_2*lpf_gain)

% gearbox rpm
gb_cutoff_freq = 10*2*pi; % [rad/s]
gb_filt_const_1 = gb_cutoff_freq/(2/control_period + gb_cutoff_freq);
gb_filt_const_2 = 1 - 2*gb_filt_const_1;
fprintf('gearbox:\nA1 = %.3f\nA2 = %.3f\n\n', gb_filt_const_1*lpf_gain, gb_filt_const_2*lpf_gain)

%% 

% create signal
n = 1000;
t = linspace(0,1,n);
y = @(x) sin(2*pi*10*x) + .5*sin(2*pi*120*x);

% y_raw = y(t);
y_raw = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 8, 8, 8, 8, 8, 8, 46, 46, 46, 46, 46, 50, 50, 50, 50, 50, 50, 50, 50, 39, 39, 39, 39, 39, 39, 39, 39, 39, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 26, 26, 26, 26, 26, 26, 26, 47, 47, 47, 47, 47, 47, 46, 46, 46, 46, 46, 46, 46, 46, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 0, 0, 0];
n = length(y_raw);
y_filt = zeros(1,n);

for i = 2:n
    y_filt(i) = eg_filt_const_1*(y_raw(i) + y_raw(i-1)) + eg_filt_const_2*y_filt(i-1)/lpf_gain;
end

plot(y_raw*10), hold on
plot(y_filt), hold off