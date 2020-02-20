% gtc_filt.m
% this script tests a digital filter on rpm data taken from the engine gear
% tooth sensor

clear;
clc;

% load data
load('eg_rpm_data.mat');

% eg_rpm_data = zeros(1000, 1);
% eg_rpm_data(1) = 1;

% filter
alpha = 2*pi * 2; % [rad/s]
ts =20e-3; % [s]
A = alpha/(2/ts+alpha);
eg_rpm_data_filt = zeros(size(eg_rpm_data));
eg_rpm_data_filt(1) = eg_rpm_data(1);
for i = 2:length(eg_rpm_data)
    eg_rpm_data_filt(i) = (1-2*A)*eg_rpm_data_filt(i-1) + A*(eg_rpm_data(i) + eg_rpm_data(i-1));
end

% plot
t = (0:length(eg_rpm_data)-1)*ts;
plot(t, eg_rpm_data, 'LineWidth', 2);
hold on;
plot(t, eg_rpm_data_filt, 'LineWidth', 2);
hold off;
xlim([0, 30]);
ylim([1500, 4000]);
xlabel('Time [s]', 'interpreter', 'latex', 'FontSize', 14);
ylabel('Engine RPM', 'interpreter', 'latex', 'FontSize', 14);
title('Filtered Engine RPM', 'interpreter', 'latex', 'FontSize', 14);
legend({'Raw', 'Filtered'}, 'interpreter', 'latex', 'location', 'northwest', 'FontSize', 14);
grid on;
