clear;
clc;

pretty_pictures();

data = readtable('chirp_data.csv');
data.Properties.VariableNames = {'t', 'vel_ref', 'pos'};
data.pos = data.pos - data.pos(1);
data.t = data.t - data.t(1);
data = data(data.t < 20, :);
N = height(data);
ts = mean(diff(data.t));
fs = 1/ts;
fnyq = fs/2;
f = (0:N-1)*fs/N;

% convert to pos_ref
pos_ref = zeros(size(data.vel_ref));
for i = 2:length(pos_ref)
    pos_ref(i) = pos_ref(i-1) + data.vel_ref(i)*ts;
end

f_lpf = 10; % [Hz]
alpha = 2*pi*f_lpf; % [rad/s]
[num, denom] = butter(1, f_lpf*2/fs);

pos_filt = filter(num, denom, data.pos);

vel = 2*pi*f_lpf*(data.pos - pos_filt);

figure(1);
plot(data.t, data.pos, '.', data.t, pos_ref, '.');
grid on;
xlabel('time [s]');
ylabel('pos [counts]');
legend({'Raw', 'Filtered'});
title('Motor position');

figure(2);
plot(data.t, vel, '.', data.t, data.vel_ref, '.');
grid on;
xlabel('time [s]');
ylabel('vel [counts/s]');
legend({'Measured', 'Reference'});
title('Motor velocity')

vel_w = fft(vel);
vel_ref_w = fft(data.vel_ref);
h = vel_w./vel_ref_w;

figure(3);
loglog(f, abs(vel_ref_w), '.', f, abs(vel_w), '.');
grid on;
xlim([.1, fnyq]);
xlabel('frequency [Hz]');
ylabel('magnitude [V/V]');
title('Input and output FFT');

figure(4);
loglog(f, abs(h), '.');
grid on;
xlim([.1, fnyq]);
xlabel('frequency [Hz]');
ylabel('magnitude [V/V]');
title('frequency response magnitude');

figure(5);
semilogx(f, 180/pi*unwrap(angle(h)), '.');
grid on;
xlim([.1, fnyq]);
xlabel('frequency [Hz]');
ylabel('phase [deg]');
title('frequency response phase');