clear;
clc;

data = readtable('data.csv');
data.Properties.VariableNames = {'t', 'vel_ref', 'pos'};
data = data(2:end,:);
data.pos = data.pos - data.pos(2);
dt = diff(data.t)/1000;
ts = mean(diff(data.t))/1000;
fs = 1/ts;

f_lpf = 10; % [Hz]
alpha = 2*pi*f_lpf; % [rad/s]
[num, denom] = butter(1, f_lpf*2/fs);

pos_filt = filter(num, denom, data.pos);

vel = zeros(size(data.vel_ref));
for i = 2:length(vel)
    vel(i) = (data.pos(i) - data.pos(i-1))/ts;
end
vel_filt = filter(num, denom, vel);

figure(1);
plot(data.t, data.pos, '.', data.t, pos_filt, '.');
grid on;
xlabel('time [s]');
ylabel('position [counts]');
title('Motor position');
figure(2);
plot(data.t, vel_filt, '.', data.t, data.vel_ref, '.');
grid on;
xlabel('time [s]');
ylabel('velocity [counts/sec]');
title('Motor speed');
