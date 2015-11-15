function [ robotCenter, R, t ] = twoPointCalc( pvect )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
dist = norm(pvect(:,1) - pvect(:,2));

robotCenter = (pvect(:,1) + pvect(:,2))/2;
R = [];
t = [];

end

