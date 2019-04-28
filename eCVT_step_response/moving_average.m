function ave = moving_average(data, n)
current_data = zeros(n, 1);
ave = zeros(size(data));
index = 0;
for i = 1:length(data)
    current_data(index + 1) = data(i);
    index = mod(index + 1, n);
    ave(i) = mean(current_data);
end
end