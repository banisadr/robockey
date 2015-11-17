function [] = Rink_Visualized(x_pos,y_pos, angle)
%Visualises where your robot is in the rink as well as the angle of
%orientation of the robot
%By Pele Collins for Robockey 2015

%This is a function that just requires the calling:
%Rink_Visualized(x_pos,y_pos, angles), where
%x_pos = Robot x position [cm]
%y_pos = Robot y position [cm]
%Angle = Angle of orientation [rad]
%NOTE THE UNITS
%Also note that if you want to plot multiple positions at the same time,
%create an array of values and then input that.

%The constellation is present in the rink as well as the origin
%marked out in the center.
%Field divisions, goals and curved edges are my estimates because no real
%values where given for these.

%The robot position is represented by a green color marker and the
%orientation by a black line from the center of the marker pointing
%outwards
%% Initializing figure and rink

figure('units','normalized','outerposition',[0 0 1 1]);
title({ ; 'Robockey Localization'}, 'FontSize',36);
xlabel('Pele Collins, 2015');

% Main Rink
pos1 = [-115,-60,230,120];
cur1 = 0.5;
rectangle('Position',pos1,'Curvature',cur1, 'FaceColor',[1,1,1],'EdgeColor','k',...
    'LineWidth',3);
axis equal
axis off

hold on

% Goal 1
pos2 = [-125,-25,10,50];
cur2 = 0;
rectangle('Position',pos2,'Curvature',cur2, 'FaceColor',[1 1 1],'EdgeColor','k',...
    'LineWidth',3);

%Goal 2
pos3 = [115,-25,10,50];
cur3 = 0;
rectangle('Position',pos3,'Curvature',cur3, 'FaceColor',[1 1 1],'EdgeColor','k',...
    'LineWidth',3);

%Constellation
Star_co_ords = [0, 14.5; 11.655, 8.741; 0. -14.5; -10.563, 2.483];
scatter(Star_co_ords(:,1),Star_co_ords(:,2),20,'r', 'filled');

%Origin
scatter(0,0,50,'k','filled');

%Compass
U = [0,5];
V = [5,0];

compass(U,V);

%Zones
lines_y = [56,-56; 60,-60; 60,-60; 56, -56];
lines_x = [-100, -100; -40, -40; 40, 40; 100, 100];

for i = 1:4
    plot(lines_x(i,:), lines_y(i,:), 'b', 'LineWidth', 1);
end


%% Plotting position of Robots and orientation

scatter(x_pos, y_pos, 'g', 'LineWidth', 2);

for i = 1:length(x_pos)
    plot([x_pos(i), x_pos(i)+4*cos(angle(i))], [y_pos(i), y_pos(i)+4*sin(angle(i))],'k', 'LineWidth', 2)
    
end


end

