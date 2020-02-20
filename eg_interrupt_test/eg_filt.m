% eg_filt.m
% this script tests a digital filter on rpm data taken from the engine hall
% sensor using interrupts

clear;
clc;

% load data
load('eg_interrupt_data.mat');

% resample to avoid repeated values
eg_interrupt_data_resamp = zeros(size(eg_interrupt_data));
eg_interrupt_data_resamp(1) = eg_interrupt_data(1);
j = 2;
for i = 2:length(eg_interrupt_data)
    if eg_interrupt_data(i) ~= eg_interrupt_data(i-1)
        eg_interrupt_data_resamp(j) = eg_interrupt_data(i);
        j = j + 1;
    end
end
eg_interrupt_data = eg_interrupt_data_resamp(1:find(eg_interrupt_data_resamp == 0)-1);

% first order gate to remove unreasonable data
for i = 2:length(eg_interrupt_data)
    if (eg_interrupt_data(i) > 4000) || (eg_interrupt_data(i) - eg_interrupt_data(i-1) > 500)
        eg_interrupt_data(i) = 2*eg_interrupt_data(i-1) - eg_interrupt_data(i-2);
    end
end

% filter
alpha = 300/(2*pi); % [rad/s]
dT = 1/45; % [s]
A = alpha/(2/dT+alpha);
eg_interrupt_data_filt = zeros(size(eg_interrupt_data));
eg_interrupt_data_filt(1) = eg_interrupt_data(1);
for i = 2:length(eg_interrupt_data)
    eg_interrupt_data_filt(i) = (1-2*A)*eg_interrupt_data_filt(i-1) + A*(eg_interrupt_data(i) + eg_interrupt_data(i-1));
end

% plot
% plot(eg_interrupt_data);
plot(eg_interrupt_data_resamp, 'LineWidth', 2);
hold on;
plot(eg_interrupt_data_filt, 'LineWidth', 2);
hold off;
xlim([0, find(eg_interrupt_data_resamp == 0, 1)]);
ylim([1500, 4000]);
ylabel('Engine RPM', 'interpreter', 'latex', 'FontSize', 14);
title('Filtered Engine Interrupt RPM', 'interpreter', 'latex', 'FontSize', 14);
legend({'Raw', 'Filtered'}, 'interpreter', 'latex', 'location', 'northwest', 'FontSize', 14);
grid on;
