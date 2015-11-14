function points = distIndex( index )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here

    points = zeros(1,2*length(index));
    
    for i = 1:length(index)
        switch index(i)
            case 1
                points(2*i-1:2*i) = [1,2];
            case 2
                points(2*i-1:2*i) = [1,3];
            case 3
                points(2*i-1:2*i) = [1,4];
            case 4
                points(2*i-1:2*i) = [2,3];
            case 5
                points(2*i-1:2*i) = [2,4];
            case 6
                points(2*i-1:2*i) = [3,4];
        end
        
    end
    
    
end



