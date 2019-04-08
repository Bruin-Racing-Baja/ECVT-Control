function plot_signal_noise(data, fs)
data = data - mean(data);
Nfft = length(data);
[Pxx, f] = pwelch(data, gausswin(Nfft), Nfft/2, Nfft, fs);
plot(f, Pxx)
end