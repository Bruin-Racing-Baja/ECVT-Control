% this script reconstructs the controller output from a time series of
% engine rpm

clear;
clc;

% actuator values
POT_MARGIN = 10;
POT_MIN = 125 + POT_MARGIN;
POT_MAX = 530 - POT_MARGIN;
POT_ENGAGE = 509;

% PWM values
PW_STOP = 1510;
PW_MIN = 1000;
PW_MAX = 2000;
u_k_min = PW_MIN - PW_STOP;
u_k_max = PW_MAX - PW_STOP;
u_k_limit = 50;

% controller values
r_k = 2700; % reference rpm value
K = 5; % controller gain
lead_z = 5.6; % lead zero
lead_p = 11.4; % lead pole
lag_z = .949; % lag zero
lag_p = .0949; % lag pole
Ts = .02;
lead_A = K*(lead_z+2/Ts)/(lead_p+2/Ts);
lead_B = K*(lead_z-2/Ts)/(lead_p+2/Ts);
lead_C = (lead_p-2/Ts)/(lead_p+2/Ts);
lag_A = (lag_z+2/Ts)/(lag_p+2/Ts);
lag_B = (lag_z-2/Ts)/(lag_p+2/Ts);
lag_C = (lag_p-2/Ts)/(lag_p+2/Ts);

% initialize values
e_k = 0;
e_k1 = 0;
lead_u_k = 0;
lead_u_k1 = 0;
u_k = 0;
u_k1 = 0;
pot = 509;

% load time series
load rpm_data_1
n = length(rpm);

% run simulation
u_ks = zeros(n,1);
pots = zeros(n,1);
for i = 1:n
    
    % measure rpm and calculate error
    rpm_i = rpm(i);
    e_k = r_k - rpm_i;
    
    % compute control signal
    lead_u_k = floor(lead_A*e_k + lead_B*e_k1 - lead_C*lead_u_k1);
    u_k = lead_u_k;
%     u_k = floor(lag_A*lead_u_k + lag_B*lead_u_k1 - lag_C*u_k1);
    u_k = max(min(u_k, u_k_max), u_k_min);
    
    % check actuator limits
    if pot >= POT_MAX
        u_k = -u_k_limit;
    elseif (pot <= POT_MIN)
        u_k = u_k_limit;
    end
    
    % update potentiometer
    pot = pot + .6239*u_k*Ts;
    
    % update past values
    e_k1 = e_k;
    lead_u_k1 = lead_u_k;
    u_k1 = u_k;
    u_ks(i) = u_k;
    pots(i) = pot;
end
t = (1:length(rpm))*.02;
plot(t, rpm), hold on, plot(t, u_ks), plot(t, pots), hold off
legend('RPM', 'u_k', 'pot');