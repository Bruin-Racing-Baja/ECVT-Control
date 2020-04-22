clear;
clc;

% constants
ts = 20e-3; % [s]
fs = 1/ts; % [Hz]
fnyq = fs/2; % [Hz]
K = 50; % position controller gain

%% chirp response

% constants
chirp_T = 20; % [s]
chirp_N = chirp_T/ts;
chirp_t = (1:chirp_N)*ts;
chirp_f0 = fs/chirp_N; % [Hz]
chirp_f = (0:chirp_N-1)*chirp_f0; % [Hz]
chirp_w = 2*pi*chirp_f; % [rad/s]

% load data
load('chirp_response.mat');
pos_ref = chirp_response.pos_ref;
pos = chirp_response.pos;

% take one period
pos_ref = pos_ref(1:chirp_N);
pos = pos(1:chirp_N);

% remove means
pos_ref = pos_ref - mean(pos_ref);
pos = pos - mean(pos);

% visualize time domain
figure(1);
plot(chirp_t, pos_ref, '.', chirp_t, pos, '.');
grid on;
xlabel('Time [s]');
ylabel('Pot position');
title('Actuator chirp response');
legend({'Reference', 'Measured'}, 'location', 'best');

% calculate transfer function
H = fft(pos)./fft(pos_ref);

% back out plant
P = 1/K * H./(1-H);
abs_P = abs(P);
ang_P = 180/pi*unwrap(angle(P));

% plant bode
figure(2);
subplot(211);
loglog(chirp_f, abs_P, '.');
ylabel('Magnitude');
xlim([.01, fnyq]);
ylim([1e-4, 1]);
grid on;
subplot(212);
semilogx(chirp_f, ang_P, '.');
xlim([.01, fnyq]);
ylim([-300, 0]);
xlabel('Frequency [Hz]');
ylabel('Phase [deg]');
grid on;
sgtitle('Actuator Bode');

%% sine response

% load data
load('sine_300mHz.mat');
load('sine_350mHz.mat');
load('sine_400mHz.mat');
sine_data = {sine_300mHz, sine_350mHz, sine_400mHz};
sine_n_tests = length(sine_data);
sine_f0 = [.3, .35, .4];
sine_f = zeros(size(sine_f0));
sine_abs_P = zeros(size(sine_f0));
sine_ang_P = zeros(size(sine_f0));

for i = 1:sine_n_tests
    data = sine_data{i};
    sine_freq = sine_f0(i);
    sine_T = 1/sine_freq;
    sine_N = round(sine_T/ts);
    sine_f_grid = (0:sine_N-1)/(sine_N*ts);
    sine_fi = find_closest(sine_f_grid, sine_freq);
    sine_f(i) = sine_f_grid(sine_fi);
    sine_pos_ref = data.pos_ref(1:sine_N);
    sine_pos = data.pos(1:sine_N);
    
%     % plot time domain to confirm
%     figure();
%     sine_t = (1:sine_N)*ts;
%     plot(sine_t, sine_pos_ref, '.', sine_t, sine_pos, '.');
%     grid on;
%     xlabel('Time [s]');
%     ylabel('Pot position');
%     legend({'Reference', 'Measured'});

    % calculate frequency response
    sine_H = fft(sine_pos)./fft(sine_pos_ref);
    sine_P = 1/K * sine_H./(1-sine_H);
    sine_abs_P(i) = abs(sine_P(sine_fi));
    sine_ang_P(i) = 180/pi*unwrap(angle(sine_P(sine_fi)));
end

% add points to bode plot
figure(2);
subplot(211);
hold on;
loglog(sine_f, sine_abs_P, '.');
vline(.35, 'k--');
hold off;
subplot(212);
hold on;
semilogx(sine_f, sine_ang_P, '.');
vline(.35, 'k--');
hold off;
% very good agreement! vertical line corresponds to approximately highest
% frequency we can control

%% controller design

% constants
f = chirp_f;
w = chirp_w;

% controller
Kp = 25;
Ki = 10;
C = Kp + Ki./(1j*2*pi*f);
% loglog(f, abs(C));
% xlim([.01, fnyq]);


% loop gain
L = C'.*P;
abs_L = abs(L);
ang_L = 180/pi*unwrap(angle(L));

% margins
lim_i = find(f == 10);
[fgc, pm, fpc, gm] = fr_margins(f, L, lim_i);
fprintf('\n');
fprintf('gain crossover: %.2f Hz\n', fgc);
fprintf('phase margin: %.2f deg\n', pm);
fprintf('gain margin: %.2f\n', gm);
fprintf('\n');

% loop gain bode
figure(3);
loglog(f, abs_L, '.');
hold on;
loglog(fpc*ones(2,1), 1./[1, gm], 'r--');
hold off;
grid on;
hline(1, 'k--');
xlim([.01, fnyq]);
vline(fgc, 'k--');
xlabel('Frequency [Hz]');
ylabel('Magnitude');
title('Loop gain');

figure(4);
semilogx(f, ang_L, '.');
hold on;
semilogx(fgc*ones(2,1) , [0, pm]-180, 'k--');
hold off;
grid on;
xlim([.01, fnyq]);
ylim([-300, 0]);
hline(-180, 'r--');
vline(fpc, 'r--');
xlabel('Frequency [Hz]');
ylabel('Phase [deg]');
title('Loop gain');