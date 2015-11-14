function [ robotCenter, R, t ] = threePointCalc( pvect )
%UNTITLED3 Summary of this function goes here
%   Detailed explanation goes here
%calculate distances between stars:
dist12 = norm(pvect(:,2) - pvect(:,1));
dist13 = norm(pvect(:,3) - pvect(:,1));
dist14 = 0;
dist23 = norm(pvect(:,3) - pvect(:,2));
dist24 = 0;
dist34 = 0;

distVect = [dist12, dist13, dist14, dist23, dist24, dist34];

[value, indexMax] = max((distVect>95) & (distVect<105));

if value
    
    pointsLong = distIndex(indexMax);
    center = (pvect(:,pointsLong(1))+pvect(:,pointsLong(2)))/2;
    
    [top, bottom] = orientationCalculation(pointsLong, distVect);
    centerLine = pvect(:,top)-pvect(:,bottom);
    
else
    [valueShort, indexShort] = max((distVect>40) & (distVect<50));
    [valueMed, indexMed] = max((distVect>50) & (distVect<60));
    
    if valueShort && valueMed 
        pointsShort = distIndex(indexShort);
        pointsMed = distIndex(indexMed);
        
        top = intersect(pointsShort,pointsMed);
        left = pointsMed(pointsMed~=top);
        right = pointsShort(pointsShort~=top);
        middle = pvect(:,left)+(10.563/(10.563+11.655))*(pvect(:,right)-pvect(:,left));
        centerLine = pvect(:,top)-middle;
        center = middle;
    else 
        error('Can not find top');
    end 
    
end

theta = atan2(-centerLine(2),centerLine(1)) + pi/2; %y is negative to compensate for pixels being flipped

R = [cos(theta), -sin(theta);
    sin(theta), cos(theta)];

t = center;

H = [R, -t; 0 0 1];
robotCenter = H*[0; 0 ; -1];

%robotCenter = R*(-t);

o_vect = H*[20; 0; -1];
line([robotCenter(1), o_vect(1)],[robotCenter(2), o_vect(2)]);

end

