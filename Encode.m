function [output] = Encode(input,mu)
%ENCODE Summary of this function goes here
%   Encodes a subband signal
%   Inputs: subband signal
%           mu
%   Output: encoded subband signal
%   Detailed explanation goes here

% mu = 1; % is received from the estimateMu function
stepsize = 1; % has to be adaptive to the input signal!
output = zeros(1,(length(input)-1));
s_star = mu*input(1);
stepsize_vec = zeros(1,length(input)-1);

for i = 1:(length(input)-1)
    delta = input(i+1) - s_star; 
    output(i) = round(delta/stepsize);
    d_prime = stepsize*output(i);
    s_prime = d_prime + s_star;
    s_star = mu*s_prime;
    if mod(i,10) == 0 && i>=10
        stepsize = StepsizeCalculation(output(i-9:i),3);
        if stepsize == 0
            stepsize = 1;
        end  
    end
    stepsize_vec(i)=stepsize;
end
% figure();
% plot(stepsize_vec); hold on
% plot(input);
end

