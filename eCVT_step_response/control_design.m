clear;
clc;
s = tf('s');

% load plant model
load ecvt_pwm2rpm

% add pade approximation of zoh lag
Ts = .02;
zoh_lag_pade = 1/(Ts/2*s + 1);

% create overall plant
sysG = zoh_lag_pade*pwm2rpm;

% design gain compensator to improve response speed
omega_s = 1/Ts * 2*pi; % sampling frequency
omega_c = omega_s/50; % upper limit of crossover frequency from sampling time
K_0 = 1/abs(evalfr(sysG, omega_c*1j));

% add phase lag to improve steady state
alpha = 3; % specify lag ratio 
phi_m = asind((1-alpha)/(1+alpha)); % calculate added phase lag
omega_m = .2; % center the lag over the large phase bump at .1 rad/s
p = omega_m/sqrt(alpha);
z = p*alpha;
K_1 = abs(evalfr(K_0*sysG, omega_m*1j))/sqrt(alpha);
