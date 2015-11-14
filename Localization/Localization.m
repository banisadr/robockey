%% Localization for Robockey 
% Pete Furlong 
% MEAM 510 
clear all
load('C:\Users\Pete\Documents\MEAM 510\mWii_training_data\C.mat');

figure(1)
hold on

alpha = 1;
robotCenterPrev = [];
RPrev = [];
tPrev = [];
%Cases: 4 stars, 3 stars, 2 starts, 1 star 

for i = 1:length(rawStarData)
    p1 = [rawStarData(i,1); rawStarData(i,5)];
    p2 = [rawStarData(i,2); rawStarData(i,6)];
    p3 = [rawStarData(i,3); rawStarData(i,7)];
    p4 = [rawStarData(i,4); rawStarData(i,8)];
    p1(2) = -1*p1(2);
    p2(2) = -1*p2(2);
    p3(2) = -1*p3(2);
    p4(2) = -1*p4(2);
    
    pvect = [p1 p2 p3 p4];
    for j = 1:4 
        if pvect(1,j) == 1023
            pvect(:,j) = 0;
        end 
    end
    a = pvect(:);
    a = a((a~=0));
    a = reshape(a,[2, length(a)/2]);
    
    POINTS = length(a);
    
    switch POINTS 
        case 4 
            [ robotCenter, R, t] = fourPointCalc(a);
        case 3 
            [ robotCenter, R, t] = threePointCalc(a);
        case 2 
            [ robotCenter, R, t] = twoPointCalc(a);
        case 1 
            [ robotCenter, R, t] = onePointCalc(a);
    end 

    if robotCenter
        robotCenterPrev = robotCenter;
        RPrev = R;
        tPrev = t;
    else
        if robotCenterPrev 
            robotCenter = robotCenterPrev;
        else 
            robotCenter = [0;0];
        end 
    end
    plot(robotCenter(1),robotCenter(2),'ro');
    pause(0.01)

    
end 

hold off

    
%     
%     %calculate distances between stars: 
%     dist12 = norm(p2 - p1); 
%     dist13 = norm(p3 - p1);
%     dist14 = norm(p4 - p1);
%     dist23 = norm(p3 - p2);
%     dist24 = norm(p4 - p2);
%     dist34 = norm(p4 - p3); 
%     
%     distVect = [dist12, dist13, dist14, dist23, dist24, dist34];
%     
%     [value, indexMax] = max((distVect>95) & (distVect<105));
% 
%     if value
%         
%         pointsLong = distIndex(indexMax);
%         center = (pvect(:,pointsLong(1))+pvect(:,pointsLong(2)))/2;
%         
%         [top, bottom] = orientationCalculation(pointsLong, distVect);
%         centerLine = pvect(:,top)-pvect(:,bottom);
%     
%     else 
%         disp('no max')
%         [valueShort, indexShort] = max((distVect>40) & (distVect<50));
%         if valueShort
%             pointsShort = distIndex(indexShort);
%             [valueMed, indexMed] = max((distVect>50) & (distVect<60));
%             if valueMed 
%                 pointsMed = distIndex(indexMed);
%                 top = intersect(pointsShort,pointsMed);
%                 center = pvect(:,top);
%                 centerLine = p1-p2;
%             end 
%         end 
%         centerLine = p1-p2;
%         center = p1;
%         
%     end 
%     
%     
%     
%     
% 
%     %plot(center(1),center(2),'kx') %plot center point 
% 
%     theta = atan2(-centerLine(2),centerLine(1)) + pi/2; %y is negative to compensate for pixels being flipped 
% 
%     R = [cos(theta), -sin(theta); 
%          sin(theta), cos(theta)];
% 
%     t = center; 
%     pRot = R*[(p1-t) (p2-t) (p3-t) (p4-t)];
%     
%     robotCenter = R*(-t);
%     
%     o_vect = R*([20; 0] -t);
%     line([robotCenter(1), o_vect(1)],[robotCenter(2), o_vect(2)]);
%     if i == 1
%         robotCenterPrev = robotCenter;
%     end 
 
%         robotCenter = alpha*robotCenter +(1-alpha)*robotCenterPrev;
