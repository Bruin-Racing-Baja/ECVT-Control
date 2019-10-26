subplot(2,1,1);
t = linspace(0, 30, 1201);
plot(t, ref(100:1300)); hold on;
plot(t, engine_rpm(100:1300)); hold off;
grid on;

ylim([2200, 3200]);

sgtitle('Bruin Racing - Model 19 - ECVT Engine Reference Mapping', 'interpreter', 'latex');
title('Controller Performance Validation', 'interpreter', 'latex');
ylabel('RPM', 'interpreter', 'latex');
legend({'Reference RPM', 'Engine RPM'}, 'interpreter', 'latex', 'location', 'best');

eg_kill = 1500;
eg_launch = 1800;
eg_torque = 2600;
eg_power = 3400;
gb_launch = 80 * .06824;
gb_torque = 128 * .06824;
gb_power = 621.6 * .06824;

x0 = linspace(0, gb_launch, 2);
x1 = linspace(gb_launch, gb_torque);
x2 = linspace(gb_torque, gb_power);
x3 = linspace(gb_power, 45, 2);

subplot(2,1,2);
plot(x0, [eg_launch, eg_launch]); hold on;
plot(x1, (eg_torque-eg_launch)/(gb_torque-gb_launch)*(x1-gb_launch) + eg_launch);
plot(x2, (eg_power-eg_torque)/(gb_power-gb_torque)*(x2-gb_torque) + eg_torque);
plot(x3, [eg_power, eg_power]); hold off;
grid on;

xlim([0, 45]);
ylim([1500, 3500]);

title('Engine Reference Mapping from Wheel Speed', 'interpreter', 'latex');
ylabel('Engine Reference RPM', 'interpreter', 'latex');
xlabel('Wheel Speed [mph]', 'interpreter', 'latex');

set(gcf, 'PaperOrientation', 'landscape');
print(gcf, '-dpdf', 'Engine_Reference_Mapping.pdf', '-fillpage');