clc;
% import the first sheet, second dataset from actuator_step_response.xlsx
plot(u2(24:123) - 1460), hold on, plot(y2(24:123) - 928), hold off
ylabel('Change in Magnitude', 'interpreter', 'latex');
xlabel('Data Point ($\Delta t = 20$ ms)', 'interpreter', 'latex');
legend({'PWM', 'Potentiometer'}, 'interpreter', 'latex');
tightfig;
% saveas(gcf, 'step_response.png');