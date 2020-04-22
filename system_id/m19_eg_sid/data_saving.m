triangle_70cps = readtable('triangle_70cps.csv');

triangle_70cps = triangle_70cps(:,1:2);

save('triangle_70cps.mat', 'triangle_70cps');