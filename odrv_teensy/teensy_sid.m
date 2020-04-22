clear;
clc;
close all;

p = 'COM23';
teensy = instrfind('Port', p);
if isempty(teensy)
    teensy = serial(p, 'BaudRate', 115200);
end
fclose(teensy);
teensy = teensy(1);
fopen(teensy);
flushinput(teensy);
flushoutput(teensy);

pause(.01);
while teensy.BytesAvailable
    flushoutput(teensy)
end
pause(.01);

ts = 10e-3;
T = 10;
N = T/ts;
win = tukeywin(N)';
win = win/(sum(win)*ts);
fb = sum(win.^2)*ts;
fs = 1/ts;
fnyq = fs/2;
f = (0:N-1)*fs/N;

Suu = zeros(1, N);
Svv = zeros(1, N);
Svu = zeros(1, N);

num_cycles = 30;
for c = 1:num_cycles

    readings = zeros(3,N);
    for i = 1:N
        try
            readings(:,i) = str2double(split(fgetl(teensy), ','));
        catch

        end
    end

    t = readings(1,:);
    vel_ref = readings(2,:);
    vel = readings(3,:);
    
    % input auto spectrum
    Suu = Suu + abs(fft(win.*vel_ref*ts)).^2/fb;
    
    % output auto spectrum
    Svv = Svv + abs(fft(win.*vel*ts)).^2/fb;
    
    % cross spectrum
    Svu = Svu + fft(win.*vel*ts).*conj(fft(win.*vel_ref*ts))/fb;
    
    % frequency response
    H = Svu./Suu;
    
    % coherence
    gamma_vu = abs(Svu)./sqrt(Suu.*Svv);

    % time domain
    figure(1);
    plot(t, vel_ref, '.', t, vel, '.');
    grid on;
    xlabel('time [s]');
    ylabel('velocity [counts/s]');
    title('time domain');
    
    % auto spectra
    figure(2);
    loglog(f, Suu/c, '.', f, Svv/c, '.');
    grid on;
    xlim([.1, fnyq]);
    xlabel('frequency [Hz]');
    ylabel('magnitude');
    title(sprintf('auto spectra N = %i', c));
    legend({'input', 'output'});
    
    % frequency response
    figure(3);
    loglog(f, abs(H), '.');
    grid on;
    xlim([.1, fnyq]);
    xlabel('frequency [Hz]');
    ylabel('magnitude');
    title(sprintf('frequency response magnitude, N = %i', c));
    
    figure(4);
    semilogx(f, 180/pi*angle(H), '.');
    grid on;
    xlim([.1, fnyq]);
    xlabel('frequency [Hz]');
    ylabel('phase [deg]');
    title(sprintf('frequency response phase, N = %i', c));
    
    % coherence
    figure(5);
    semilogx(f, abs(gamma_vu));
    grid on;
    xlim([.1, fnyq]);
    ylim([0, 1]);
    xlabel('frequency [Hz]');
    ylabel('magnitude');
    title(sprintf('coherence, N = %i', c));

end

Suu = Suu/c;
Svv = Svv/c;
Svu = Svu/c;

fclose(teensy);