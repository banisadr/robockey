function [top, bottom] = orientationCalculationFour(realLongIndex, distVect)
%UNTITLED3 Summary of this function goes here
%   Detailed explanation goes here
[value, indexShort] = max((distVect>40) & (distVect<50));

if value
    
    pointsShort = distIndex(indexShort);
    
    top = intersect(pointsShort,realLongIndex);
    
    if top
        bottom = realLongIndex(realLongIndex~=top);
    else
        disp('no min')
        [value2, indexMed] = max((distVect>50) & (distVect<60));
        if value2
            
            pointsMed = distIndex(indexMed);
            
            top = intersect(pointsMed,realLongIndex);
            bottom = realLongIndex(realLongIndex~=top);
        else
            top = 1; bottom = 2;
        end
    end
else
    disp('no min')
    [value2, indexMed] = max((distVect>50) & (distVect<60));
    if value2
        
        pointsMed = distIndex(indexMed);
        
        top = intersect(pointsMed,realLongIndex);
        bottom = realLongIndex(realLongIndex~=top);
    else
        disp('no med')
        top = 1;
        bottom = 2;
    end
end

