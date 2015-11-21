function [top, bottom] = orientationCalculationFour(realLongIndex, distVect)
%UNTITLED3 Summary of this function goes here
%   Detailed explanation goes here
[value, indexShort] = min(distVect);

if value
    
    pointsShort = distIndex(indexShort);
    pointsShort
    
    top = intersect(pointsShort,realLongIndex);
    
    if top
        bottom = realLongIndex(realLongIndex~=top);
    else
        top = 0;
        bottom = 0;
    end
else
    error('no shortest distance');
end

