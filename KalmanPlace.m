%% Kalman Filter
%{
 Ben Davis
 5-14-22
 This script uses a Kalman Filter to smooth a random data set.
 This type of Kalman Filtering will only work with linear data.
 This type of Kalman Filtering does not require continuous functions.
%}

clc; clear all; close all;

% Create an array of random data
DataLength = 1000; 
unfilteredData = rand(1,DataLength); 


% Add variations to the data
if true
    i = 1;
    while i <= int16(0.5*length(unfilteredData))
        unfilteredData(i) = unfilteredData(i) + 1;
        i = i + 1;
    end

    while i <= length(unfilteredData)
        unfilteredData(i) = unfilteredData(i) - 1;
        i = i + 1;
    end
end
if true
    i = 100;
    while i <= 300
        unfilteredData(i) = unfilteredData(i) + 5 ;
        i = i + 1;
    end

    i = 800;
    while i<=900
        unfilteredData(i) = unfilteredData(i) - 5;
        i = i + 1;
    end

    i = 400;
    while i<=600
        unfilteredData(i) = unfilteredData(i) + 3;
        i = i + 1;
    end
end

% Create a plotting placeholder for the x axis
xAxis = [1:1:length(unfilteredData)];


%% Filtering Data
errorEst = 1; % Expected
errorMes = .05; % Measurement
% For our baro sensor specifically, from data sheets
KG = errorEst / (errorEst + errorMes);

Estimate = rand(1,DataLength);
Estimate(1) = unfilteredData(1);


i = 2;
iold = 0;
while i <= length(unfilteredData)
    KG = errorEst / (errorEst + errorMes);
    Estimate(i) = Estimate(i-1) + KG*(unfilteredData(i) - Estimate(i-1));
    errorEst = (1-KG)*errorEst;

    % Check if the value changes significantly
    if (abs(unfilteredData(i)) > abs(unfilteredData(i-1)) + errorMes ) || (abs(unfilteredData(i)) < abs(unfilteredData(i-1)) - errorMes)
      
        errorEst = abs(unfilteredData(i) - unfilteredData(i-1));

        % Check to see how if the error is reset too often  
        inew = i;
        if (inew < iold + 50)
            fprintf("Error of Measurements is too large at %i \n",i)
        end
        iold = inew;

    end

    i = i + 1;
end %%


%% Graphing

tiledlayout(2,1)
% Top plot
nexttile
plot(xAxis,unfilteredData)
title('Unfiltered Data')
grid on
% Bottom plot
nexttile
plot(xAxis,Estimate)
title('Filtered Data')
grid on


