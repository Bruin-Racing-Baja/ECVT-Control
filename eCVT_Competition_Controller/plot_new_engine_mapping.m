clear;
clc;

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