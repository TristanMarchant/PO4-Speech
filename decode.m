% Function that decodes an encoded subband
% Inputs:   - input is the encoded subband signal that has to be decoded
%           - mu is the mu used for this subband (calculated in the
%           estimateMu function.
%           - no_bits are the number of bits per sample used for this
%           subband (specified in the main function)
% Output:   - the decoded subband signal

function [output] = decode(input,mu,no_bits)

%initialization
stepsize = 1;
output = zeros(1, length(input));
delta_prime_array = zeros(1, length(input));

if no_bits ~= 0   
    for i = 2:length(input)    
        
        % calculate delta_prime (i.e. dequantized estimation error)
        delta_prime = round(input(i) * stepsize);
        delta_prime_array(i) = delta_prime;

        % update stepsize
        if i>10
            stepsize = StepsizeCalculation(delta_prime_array(i-9:i),no_bits);
            if stepsize == 0
                stepsize = 1;
            end  
        end
      
        % calculate the output
        output(i) = round(delta_prime + (mu * output(i-1))/2^15);
    end
end