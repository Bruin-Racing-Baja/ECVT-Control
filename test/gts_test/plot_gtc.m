clear;
clc;

load('gtc.mat');

plot(gtc);
grid on
xlabel('index', 'interpreter', 'latex', 'FontSize', 14)
ylabel('gear tooth count', 'interpreter', 'latex', 'FontSize', 14)
title('gear tooth sensor interrupt test')
title('gear tooth sensor interrupt test', 'interpreter', 'latex', 'FontSize', 14)
% saveas(gcf, 'gtc_test.png');