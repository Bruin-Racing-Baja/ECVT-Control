clc;
% import the last sheet, all datasets from actuator_step_response.xlsx

ax1 = subplot(3,1,1);
plot(u_raw), hold on, plot(y_raw), hold off
title('No Filter', 'interpreter', 'latex');

ax2 = subplot(3,1,2);
plot(u_dig), hold on, plot(y_dig), hold off
title('Digital LPF ($\omega_c = 22$ rad/s)', 'interpreter', 'latex');

ax3 = subplot(3,1,3);
plot(u_anal), hold on, plot(y_anal), hold off
title('Analog LPF ($\omega_c = 22$ rad/s)', 'interpreter', 'latex');

linkaxes([ax1, ax2, ax3], 'xy');
xlim([0 250]);

saveas(gcf, 'filter_comparison.png');