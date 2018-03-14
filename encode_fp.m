% Function that encodes a subband signal
% Inputs:   - input: subband signal that has to be encoded
%           - mu: mu used for this subband (from the estimateMu function)
%           - no_bits: number of bits per sample used for he specific
%           subband
% Output:   - the encoded subband signal

function [output] = encode_fp(input,mu,no_bits)

%initialization
prediction = round(mu * input(1)/2^16); %(16,14)*(16,15) format => prediction in (16,13)
codebook = -2^(no_bits-1):1:(2^(no_bits-1)-1);
stepsize = 1;
output = zeros(1, length(input));
delta_prime_array = zeros(1, length(input));
if no_bits ~=0   
    for i = 2:length(input)
        delta = round(input(i) - prediction/(2^2)); % (16.15)-(16.13)/2^2
        
        %quantize delta
        under_edge = (-2^(no_bits-1) + 0.5) * stepsize;
        upper_edge = (2^(no_bits-1) - 1.5 ) * stepsize;
        partition = under_edge:stepsize:upper_edge; 
        [~,delta_quantized] = quantiz(delta,partition,codebook);
        output(1,i) = delta_quantized;
        
        %calculate delta prime
        delta_prime = round(delta_quantized * stepsize/2^16);
        delta_prime_array(i) = delta_prime;

        % update the stepsize
        if i>10
            stepsize = StepsizeCalculation_fp(delta_prime_array(i-9:i),no_bits);
            if stepsize == 0
                stepsize = 1;
            end  
        end
       
        % calculate new prediction
        prediction = round(mu * (delta_prime + prediction)/2^16);
    end
end