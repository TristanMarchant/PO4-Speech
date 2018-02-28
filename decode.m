function [ output ] = decode(input,mu)
%DECODE Summary of this function goes here
%   Decode the encoded subband signal
%   Detailed explanation goes here

%   Calculate the stepsize based on the quantised samples (same method as
%   encode to get the same stepsizes and that way the original signal).

mu = 1;
stepsize = 0.01;
output = zeros(1,length(input)+1);

for i = 1:(length(input)+1)
    %TODO Calculate stepsize based on input
    
    if ((length(input)+1) ~= i)
        d_prime = stepsize * input(i);
        if mod(i,500) == 0 && i>500
            stepsize = StepsizeCalculation(input(i-500:i),3);
        end
    end   
    s_star = mu*output(i);
    output(i) = d_prime + s_star;
end

