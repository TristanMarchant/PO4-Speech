function [output, delta_prime_array, stepsize_array] = DecodeEllen(input,mu,no_bits)
%initialization
stepsize = 1;
output = zeros(1, length(input));
delta_prime_array = zeros(1, length(input));
stepsize_array = zeros(1, length(input));
stepsize_array(1) = stepsize;

for i = 2:length(input)    
    % calculate delta_prime (i.e. dequantized estimation error)
    delta_prime = ( input(i) - 2^(no_bits-1) ) * stepsize;
    delta_prime_array(i) = delta_prime;
    
    % each 10 samples, update the stepsize
    if mod(i,10) == 0
        stepsize = StepsizeCalculation(delta_prime_array(i-9:i),no_bits);
        if stepsize == 0
            stepsize = 1;
        end  
    end
    stepsize_array(i) = stepsize;
    
    % calculate the output
    output(i) = delta_prime + mu * output(i-1);
end
end