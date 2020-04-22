clear;
clc;

data = readtable('data.csv');
data.Properties.VariableNames = {'t', 'vel_ref', 'pos'};
data = data(1:end-1, :);
data.pos = data.pos - data.pos(1);
dt = mean(diff(data.t))/1000;
fs = 1/dt;

gate = 42; % [counts]
pos_gated = zeros(size(data.pos));
pos_gated(1) = data.pos(1);
for i = 2:length(data.pos)
    if abs(data.pos(i) - data.pos(i-1)) > 42
        pos_gated(i) = pos_gated(i-1);
    else
        pos_gated(i) = data.pos(i);
    end
end

% plot(data.t, data.pos, '.', data.t, pos_gated, '.');

f_lpf = 10; % [Hz]
alpha = 2*pi*f_lpf; % [rad/s]
[num, denom] = butter(1, f_lpf*2/fs);

pos_filt = filter(num, denom, pos_gated);

vel = 2*pi*f_lpf*(pos_gated - pos_filt);

figure(1);
plot(data.t, pos_gated, '.', data.t, pos_filt, '.');
figure(2);
plot(data.t, vel, '.', data.t, data.vel_ref, '.');
