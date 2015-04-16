ser = serial('/dev/ttyUSB0', 'BaudRate', 115200);

fopen(ser);

figure
x = 0:99;
y = zeros(1,100);
plot(x,y); hold on

while 1,
    line = fgetl(ser);
    [ax, line] = strtok(line, ':');
    [ay, line] = strtok(line, ':');
    [az, line] = strtok(line, ':');
    [gx, line] = strtok(line, ':');
    [gy, line] = strtok(line, ':');
    [gz, line] = strtok(line, ':');
    ax = hex2dec(ax);
    ax = (bitand(ax, 2^15-1) - bitand(ax, 2^15)*2)/16384;
    x = [x(2:end), x(end)+1];
    y = [y(2:end), ax];
    plot(x, y);
end

fclose(ser);