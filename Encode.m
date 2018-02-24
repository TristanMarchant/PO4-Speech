function [output] = Encode(input,mu)
%ENCODE Summary of this function goes here
%   Encodes a subband signal
%   Inputs: subband signal
%           mu
%   Output: encoded subband signal
%   Detailed explanation goes here

mu = 1; % is received from the estimateMu function
stepsize = 0.01; % has to be adaptive to the input signal!
output = zeros(1,(length(input)-1));
s_star = mu*input(1);

for i = 1:(length(input)-1)
    delta = input(i+1) - s_star; 
    output(i) = round(delta/stepsize);
    d_prime = stepsize*output(i);
    s_prime = d_prime + s_star;
    s_star = mu*s_prime;
    %TODO Code to adapt stepsize
end
end

