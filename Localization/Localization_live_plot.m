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
M2USB = serial('COM5','Baudrate', 9600);
% *** Use the device manager to check where the microcontroller is plugged
% into.

%----> for ***MAC***
% M2USB = serial('/dev/tty.usbmodem411','Baudrate',9600);
% *** Check where your device is by opening terminal and entering the command:
% 'ls /dev/tty.usb' and tab-completing.

fopen(M2USB);                                   % Open up the port to the M2 microcontroller
flushinput(M2USB);                              % Flush the input buffer

% Send initial packet to get first set of data from microcontroller                               % Send a packet to the M2
time = 0;                                       % Set the start time to 0
i = 1;                                          % Start the indexer at 1
tic;                                            % Start timer.

%% Plotting
figure();
clf;
suptitle('IMU values');        
% plot(x,'XDataSource','real(x)','YDataSource','imag(x)')
subplot(3,1,1);
hold on
xlabel('Time');
ylabel('Acceleration');        
axis([0 maxPoints -(2^14) 2^14]);
ax = plot(t, IMU_ax,':*r','LineWidth',2);
ay = plot(t, IMU_ay,':*g','LineWidth',2);
az = plot(t, IMU_az,':*b','LineWidth',2);
ax.XDataSource = 't';
ax.YDataSource = 'IMU_ax';
ay.XDataSource = 't';
ay.YDataSource = 'IMU_ay';
az.XDataSource = 't';
az.YDataSource = 'IMU_az';
legend('ax', 'ay', 'az');
grid on;
grid minor;

subplot(3,1,2);
hold on
xlabel('Time');
ylabel('Gyro');  
axis([0 maxPoints -(2^15) 2^15]);
gx = plot(t, IMU_gx,':*r','LineWidth',2);
gy = plot(t, IMU_gy,':*g','LineWidth',2);
gz = plot(t, IMU_gz,':*b','LineWidth',2);
gx.XDataSource = 't';
gx.YDataSource = 'IMU_gx';
gy.XDataSource = 't';
gy.YDataSource = 'IMU_gy';
gz.XDataSource = 't';
gz.YDataSource = 'IMU_gz';
legend('gx', 'gy', 'gz');
grid on;
grid minor;

subplot(3,1,3);
hold on
xlabel('Time');
ylabel('Magnetometer');
axis([0 maxPoints -(2^10) 2^10]);
mx = plot(t, IMU_mx,'--r','LineWidth',2);
my = plot(t, IMU_my,'--g','LineWidth',2);
mz = plot(t, IMU_mz,'--b','LineWidth',2);
mx.XDataSource = 't';
mx.YDataSource = 'IMU_mx';
my.XDataSource = 't';
my.YDataSource = 'IMU_my';
mz.XDataSource = 't';
mz.YDataSource = 'IMU_mz';
legend('mx', 'my', 'mz');
grid on;
grid minor;

% Send initial confirmation packet
fwrite(M2USB,1);                                % Confirmation packet

%% Run program forever
try
    while 1
        
        %% Read in data and send confirmation packet
        m2_buffer = fgetl(M2USB);               % Load buffer
        fwrite(M2USB,1);                        % Confirmation packet
        
        %% Parse microcontroller data
        [m2_ax, remain] = strtok(m2_buffer);
        [m2_ay, remain2] = strtok(remain);
        [m2_az, remain3] = strtok(remain2);
        [m2_gx, remain4] = strtok(remain3);
        [m2_gy, remain5] = strtok(remain4);
        [m2_gz, remain6] = strtok(remain5);
        [m2_mx, remain7] = strtok(remain6);
        [m2_mz, remain8] = strtok(remain7);
        [m2_my] = strtok(remain8);
        m2_buffer;
        time = toc;                             % Stamp the time the value was received
        
        % Remove the oldest entry 
        IMU_ax = [str2double(m2_ax) IMU_ax(1:maxPoints-1)] ;
        IMU_ay = [str2double(m2_ay) IMU_ay(1:maxPoints-1)] ;
        IMU_az = [str2double(m2_az) IMU_az(1:maxPoints-1)] ;
        IMU_gx = [str2double(m2_gx) IMU_gx(1:maxPoints-1)] ;
        IMU_gy = [str2double(m2_gy) IMU_gy(1:maxPoints-1)] ;
        IMU_gz = [str2double(m2_gz) IMU_gz(1:maxPoints-1)] ;
        IMU_mx = [str2double(m2_mx) IMU_mx(1:maxPoints-1)] ;
        IMU_my = [str2double(m2_my) IMU_my(1:maxPoints-1)] ;
        IMU_mz = [str2double(m2_mz) IMU_mz(1:maxPoints-1)] ;

        % Update plots
        refreshdata;
        drawnow;
        
        pause(.0001);
        
        hold off
        
        i=i+1;
    end
catch ME
    ME.stack
    fclose(M2USB);                              % Close serial object
end
