% plots the engine reference rpm as a function of gearbox rpm

clear;
clc;

%% old version

eg_idle = 1750;
eg_engage = 2100;
eg_launch = 2600;
eg_torque = 2700;
eg_power = 3400;

gb_launch = 80;
gb_torque = 128;
gb_power = 621.6;

% if rpm < launch: r = launch
% if rpm > power: r = power

gb_points = [-1, gb_launch, gb_torque, gb_power, 1000] * .0541;
eg_points = [eg_launch, eg_launch, eg_torque, eg_power, eg_power];

gb_rpm = (-1:750) * .0541;

plot(gb_rpm, linterp(gb_points, eg_points, gb_rpm)), hold on;
plot(gb_points, eg_points, 'bo');
plot([0, 750*.0541], [3750, 3750], '-'), hold off;
xlim([0, 40]);
ylim([2500, 3800]);
xlabel('Wheel speed [miles/hour]', 'interpreter', 'latex');
ylabel('Engine speed [rev/min]', 'interpreter', 'latex');

saveas(gcf, 'reference_mapping_old.png')

%% new version

EG_IDLE = 1750;
EG_LAUNCH = 2600;
EG_POWER = 3400;

GB_LAUNCH = 80;
GB_POWER = 621.6;

gb_points = [0, GB_LAUNCH, GB_POWER, 50/.0541] * .0541;
eg_points = [EG_LAUNCH, EG_LAUNCH, EG_POWER, EG_POWER];

plot([0, gb_points(end)], [1700, 1700], 'k--'), hold on;
plot([0, gb_points(end)], [3750, 3750], 'r--');
plot(gb_points, eg_points, 'b');
hold off
grid on
xlabel('Wheel speed [mph]');
ylabel('Engine reference [RPM]');
legend({'Idle RPM', 'Max RPM', 'Mapping'}, 'location', 'best');

saveas(gcf, 'reference_mapping_new.png')
