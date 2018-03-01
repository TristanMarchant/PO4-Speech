function [ output ] = decode(input,mu)
%DECODE Summary of this function goes here
%   Decode the encoded subband signal
%   Detailed explanation goes here

%   Calculate the stepsize based on the quantised samples (same method as
%   encode to get the same stepsizes and that way the original signal).

%mu = 1;
stepsize = 1;
output = zeros(1,length(input)+1);

for i = 1:(length(input)+1)
    
    if ((length(input)+1) ~= i)
        d_prime = stepsize * input(i);
        if mod(i,10) == 0 && i>=10
            stepsize = StepsizeCalculation(input(i-9:i),3);
            if stepsize == 0
                stepsize = 1; 
            end  
        end
    end   
    s_star = mu*output(i);
    output(i) = d_prime + s_star;
end

