%% Initialize program and USB port
if(exist('M2USB'))
  fclose(M2USB);
 else
%  fclose(instrfindall);
 delete(instrfindall);
end

%% If the above initialization does not work, please run the following commands manually and disconnect and reconnect USB.
%  fclose(serial('COM5','Baudrate', 9600));
%  fclose(instrfindall);
clear all;
close all;


%% VARIABLES
maxPoints = 50;                                 % Max number of data points displayed at a time.
t = 1:1:maxPoints;                              % Create an evenly spaced matrix for X-axis.
x_plot = zeros(1,maxPoints);                    % Pre-allocate arrays for IMU values
y_plot = zeros(1,maxPoints);

%% SERIAL
%----> for ***WINDOWS***
M2USB = serial('COM7','Baudrate', 9600);
% *** Use the device manager to check where the microcontroller is plugged
% into.

%----> for ***MAC***
% M2USB = serial('/dev/tty.usbmodem411','Baudrate',9600);
% *** Check where your device is by opening terminal and entering the command:
% 'ls /dev/tty.usb' and tab-completing.

fopen(M2USB);                                   % Open up the port to the M2 microcontroller
flushinput(M2USB);                              % Flush the input buffer
fwrite(M2USB,1);
% Send initial packet to get first set of data from microcontroller                               % Send a packet to the M2
time = 0;                                       % Set the start time to 0
i = 1;                                          % Start the indexer at 1
tic;                                            % Start timer.

%% Plotting
figure();
clf;
title('Location');        
% plot(x,'XDataSource','real(x)','YDataSource','imag(x)')
hold on        
axis([-512 512 -768/2 768/2]);
position = plot(x_plot, y_plot,'ro');
%orientation = plot([0 0],[0 0],'b-');
o_vectx = [0 0];
o_vecty = [0 0];
position.XDataSource = 'x_plot';
position.YDataSource = 'y_plot';
%orientation.XDataSource = 'o_vectx';
%orientation.YDataSource = 'o_vecty';

grid on;
grid minor;

%% Run program forever
% try
    while 1
        
        %% Read in data and send confirmation packet
        m2_buffer = fgetl(M2USB);               % Load buffer
        fwrite(M2USB,1);
        
        %% Parse microcontroller data
%        m2_buffer
        [x1, remain] = strtok(m2_buffer);
%         [x2, remain2] = strtok(remain);
%         [x3, remain3] = strtok(remain2);
%         [x4, remain4] = strtok(remain3);
%         [y1, remain5] = strtok(remain4);
%         [y2, remain6] = strtok(remain5);
%         [y3, remain7] = strtok(remain6);
        [y1] = strtok(remain);
        m2_buffer;
        time = toc;                             % Stamp the time the value was received
        x1 = str2double(x1);
        y1 = str2double(y1);
        
        % Convert star data to doubles
        
%         rawStarData = [str2double(x1),...
%                        str2double(x2),...
%                        str2double(x3),...
%                        str2double(x4),...
%                        str2double(y1),...
%                        str2double(y2),...
%                        str2double(y3),...
%                        str2double(y4)];
                   
%          rawStarData;
%         [x_point, y_point, o_vect] = Localization_function(rawStarData, [x_plot(1); y_plot(1); 1]);
        
        % Remove the oldest entry 
        x_plot = [x1 x_plot(1:maxPoints-1)] ;
        y_plot = [y1 y_plot(1:maxPoints-1)] ;
        
%         if o_vect 
%             o_vectx = o_vect(1,:);
%             o_vecty = o_vect(2,:);
%         else 
%             o_vectx = [0 0];
%             o_vecty = [0 0];
%         end 

        % Update plots
        refreshdata;
        drawnow;
        
%         pause(.0001);
        
        hold off
        
        i=i+1;
    end
% catch ME
%     ME.stack
%     fclose(M2USB);                              % Close serial object
%     error('some shit just happened')
% end
