clear;
clc;
pretty_pictures();

load validation_data

ref = validation_data.ref;
engine_rpm = validation_data.engine_rpm;

N = length(ref);
t0 = 20e-3;
t = (0:N-1)*t0;

grid on;

plot(t, ref, t, engine_rpm);
grid on;
xlim([0, 25]);

% sgtitle('Bruin Racing - Model 19 - ECVT Engine Reference Mapping', 'interpreter', 'latex');
% title('Controller Performance Validation', 'interpreter', 'latex');
xlabel('Time [s]');
ylabel('RPM');
legend({'Reference RPM', 'Engine RPM'}, 'location', 'southeast');

% eg_kill = 1500;
% eg_launch = 1800;
% eg_torque = 2600;
% eg_power = 3400;
% gb_launch = 80 * .06824;
% gb_torque = 128 * .06824;
% gb_power = 621.6 * .06824;

% x0 = linspace(0, gb_launch, 2);
% x1 = linspace(gb_launch, gb_torque);
% x2 = linspace(gb_torque, gb_power);
% x3 = linspace(gb_power, 45, 2);

% subplot(2,1,2);
% plot(x0, [eg_launch, eg_launch]); hold on;
% plot(x1, (eg_torque-eg_launch)/(gb_torque-gb_launch)*(x1-gb_launch) + eg_launch);
% plot(x2, (eg_power-eg_torque)/(gb_power-gb_torque)*(x2-gb_torque) + eg_torque);
% plot(x3, [eg_power, eg_power]); hold off;
% grid on;

% xlim([0, 45]);
% ylim([1500, 3500]);

% title('Engine Reference Mapping from Wheel Speed', 'interpreter', 'latex');
% ylabel('Engine Reference RPM', 'interpreter', 'latex');
% xlabel('Wheel Speed [mph]', 'interpreter', 'latex');

% set(gcf, 'PaperOrientation', 'landscape');
% print(gcf, '-dpdf', 'Engine_Reference_Mapping.pdf', '-fillpage');