clc; close all
datos = dlmread('data.csv',';');
datos = datos(1:end-2,:);
theta = atan2(datos(:,2), datos(:,3))*180/pi;

alpha = 0.2;
theta2 = filter(alpha, [1, -(1-alpha)], theta);

plot(theta); hold on
plot(theta2,'r');

ang = zeros(1, length(datos(:,4)));
ang(1) = theta(2);

for i=2:length(datos(:,4))
    ang(i) = ang(i-1) + datos(i-1,4)*1/200;
end

% figure
plot(ang,'g')

figure
plot(datos(:,1:3));
var(datos(:,1))

figure
plot(datos(:,4:6))