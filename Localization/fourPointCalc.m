function [ robotCenter, R, t ] = fourPointCalc( pvect )
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here

%calculate distances between stars:
dist12 = norm(pvect(:,2) - pvect(:,1));
dist13 = norm(pvect(:,3) - pvect(:,1));
dist14 = norm(pvect(:,4) - pvect(:,1));
dist23 = norm(pvect(:,3) - pvect(:,2));
dist24 = norm(pvect(:,4) - pvect(:,2));
dist34 = norm(pvect(:,4) - pvect(:,3));

distVect = [dist12, dist13, dist14, dist23, dist24, dist34];

[value, indexMax] = max(distVect);

if value
    
    pointsLong = distIndex(indexMax);
    center = (pvect(:,pointsLong(1))+pvect(:,pointsLong(2)))/2;
    
    [top, bottom] = orientationCalculationFour(pointsLong, distVect);
    
else
    error('Can not find max distance');
    
end

if top && bottom

    centerLine = pvect(:,top)-pvect(:,bottom);
    theta = atan2(-centerLine(2),centerLine(1)) + pi/2; %y is negative to compensate for pixels being flipped

    R = [cos(theta), -sin(theta);
        sin(theta), cos(theta)];

    t = -center+[1023/2; 768/2];
    H1 = [-1 0 0; 0 1 0; 0 0 1];
    H2 = [R, [0; 0]; 0 0 1];
    H3 = [eye(2), t; 0 0 1];
    %H = [R', -t; 0 0 1];
    H = H1*H2*H3;
    robotCenter = H*[0; 0; 1];
    %robotCenter(2) = -robotCenter(2);
    %robotCenter = R*(-t);

    o_vect = H*[100; 0; 1];
    %line([robotCenter(1), o_vect(1)],[robotCenter(2), o_vect(2)]);
else
    robotCenter = [];
    R = [];
    t = [];
end


end

