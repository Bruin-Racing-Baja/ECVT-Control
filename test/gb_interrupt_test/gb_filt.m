% gb_filt.m
% this script tests a digital filter on rpm data taken from the gearbox

clear;
clc;

% load data
load('gb_interrupt_data.mat');

% resample to avoid repeated values
gb_interrupt_data_resamp = zeros(size(gb_interrupt_data));
gb_interrupt_data_resamp(1) = gb_interrupt_data(1);
j = 2;
for i = 2:length(gb_interrupt_data)
    if gb_interrupt_data(i) ~= gb_interrupt_data(i-1)
        gb_interrupt_data_resamp(j) = gb_interrupt_data(i);
        j = j + 1;
    end
end
gb_interrupt_data = gb_interrupt_data_resamp(1:find(gb_interrupt_data_resamp == 0)-1);

% first order gate to remove unreasonable data
for i = 2:length(gb_interrupt_data)
    if (gb_interrupt_data(i) > 1000) || (gb_interrupt_data(i) - gb_interrupt_data(i-1) > 500)
        gb_interrupt_data(i) = 2*gb_interrupt_data(i-1) - gb_interrupt_data(i-2);
    end
end

% filter
alpha = 300/(2*pi); % [rad/s]
dT = 1/45; % [s]
A = alpha/(2/dT+alpha);
gb_interrupt_data_filt = zeros(size(gb_interrupt_data));
gb_interrupt_data_filt(1) = gb_interrupt_data(1);
for i = 2:length(gb_interrupt_data)
    gb_interrupt_data_filt(i) = (1-2*A)*gb_interrupt_data_filt(i-1) + A*(gb_interrupt_data(i) + gb_interrupt_data(i-1));
end

% plot(gb_interrupt_data);
plot(gb_interrupt_data_resamp, 'LineWidth', 2);
hold on;
plot(gb_interrupt_data_filt, 'LineWidth', 2);
hold off;
xlim([0, find(gb_interrupt_data_resamp == 0, 1)]);
ylim([0, 1000]);
ylabel('Gearbox RPM', 'interpreter', 'latex', 'FontSize', 14);
title('Filtered Gearbox Interrupt RPM', 'interpreter', 'latex', 'FontSize', 14);
legend({'Raw', 'Filtered'}, 'interpreter', 'latex', 'location', 'northwest', 'FontSize', 14);
grid on;
