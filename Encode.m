function [output,stepsize_vec] = Encode(input,mu,no_bits)
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
    codebook = (-no_bits -1):1:no_bits;
    partition = ((-no_bits+0.5)):1:((no_bits-0.5));
    index = quantiz(delta/stepsize,partition);
    output(i) = codebook(index+1);
    %output(i) = round(delta/stepsize);
    d_prime = stepsize*output(i);
    s_prime = d_prime + s_star;
    s_star = mu*s_prime;
    if mod(i,10) == 0 && i>=10
        stepsize = StepsizeCalculation(output(i-9:i),no_bits);
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

