clear;
clc;

% constants
ts = 20e-3;
fs = 1/ts;

% load data
load('eg_noise_sine.mat');
N = size(eg_noise_sine,1);
t = (1:N)*ts;
pos_ref = eg_noise_sine.pos_ref;
pos = eg_noise_sine.pos;
eg_rpm = eg_noise_sine.eg_rpm;

plot(t, pos_ref, t, pos/mean(pos), t, eg_rpm);
% plot(t, pos_ref/mean(pos_ref), t, pos/mean(pos), t, eg_rpm/mean(eg_rpm));
grid on;