% this script tests the effect of computing a control signal via z
% transform on the entire controller in one step vs series connection of
% discrete lead followed by discrete lag

%% setup

clear;
clc;

% PWM values
PW_STOP = 1510;
PW_MIN = 1000;
PW_MAX = 2000;
u_k_min = PW_MIN - PW_STOP;
u_k_max = PW_MAX - PW_STOP;

% controller values
r_k = 2700; % reference rpm value
rpm = 3700; % initial rpm value
K = 5; % controller gain
lead_z = 5.602; % lead zero
lead_p = 11.43; % lead pole
lag_z = .9487; % lag zero
lag_p = .09487; % lag pole
Ts = .02;
lead_A = K*(lead_z+2/Ts)/(lead_p+2/Ts);
lead_B = K*(lead_z-2/Ts)/(lead_p+2/Ts);
lead_C = (lead_p-2/Ts)/(lead_p+2/Ts);
lag_A = (lag_z+2/Ts)/(lag_p+2/Ts);
lag_B = (lag_z-2/Ts)/(lag_p+2/Ts);
lag_C = (lag_p-2/Ts)/(lag_p+2/Ts);

u_k = 0;
u_k1 = 0;
u_k2 = 0;
e_k = 0;
e_k1 = 0;
e_k2 = 0;

% simulation parameters
T_final = 10;
u_ks = zeros(T_final/Ts, 1);

%% combined lead-lag

for i = 1:T_final/Ts
    e_k = r_k - rpm;
    u_k = (K * ((1+Ts/2*lead_z)*(1+Ts/2*lag_z)*e_k + (-2+Ts^2/2*lead_z*lag_z)*e_k1 + (1-Ts/2*lead_z)*(1-Ts/2*lag_z)*e_k2) - (1+Ts/2*lead_p)*(1+Ts/2*lag_p)*u_k1 - (1-Ts/2*lead_p)*(1-Ts/2*lag_p)*u_k2) / ((1+Ts/2*lead_p)*(1+Ts/2*lag_p));
    u_k = max(min(u_k, u_k_max), u_k_min);
    rpm = rpm + u_k*.6239*Ts;
    
    e_k2 = e_k1;
    e_k1 = e_k;
    u_k2 = u_k1;
    u_k1 = u_k;
    u_ks(i) = u_k;
end

figure(1);
plot(u_ks);

%% series lead-lag

lead_u_k = 0;
lead_u_k1 = 0;

for i = 1:T_final/Ts
    e_k = r_k - rpm;
    lead_u_k = floor(lead_A*e_k + lead_B*e_k1 - lead_C*lead_u_k1);
    u_k = floor(lag_A*lead_u_k + lag_B*lead_u_k1 - lag_C*u_k1);
    u_k = max(min(u_k, u_k_max), u_k_min);
    rpm = rpm + u_k*.6239*Ts;
    
    e_k1 = e_k;
    lead_u_k1 = lead_u_k;
    u_k1 = u_k;
    u_ks(i) = u_k;
end

figure(2);
plot(u_ks + PW_STOP);