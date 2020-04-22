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

saveas(gcf, 'validation_data.png');