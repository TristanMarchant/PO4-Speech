% Function that encodes a subband signal
% Inputs:   - input: subband signal that has to be encoded
%           - mu: mu used for this subband (from the estimateMu function)
%           - no_bits: number of bits per sample used for the specific
%           subband
% Output:   - the encoded subband signal

function [output] = encode(input,mu,no_bits)

% Initialization
prediction = round((mu * input(1))/2^15);
codebook = -2^(no_bits-1):1:(2^(no_bits-1)-1);
stepsize = 1;
output = zeros(1, length(input));
delta_prime_array = zeros(1, length(input));

if no_bits ~= 0   
    for i = 2:length(input)
        % Multiplied by 2 to make the partition integer numebers
        delta = round((input(i) - prediction) * 2);
        
        % Quantize delta
        under_edge = round((-2^(no_bits-1) + 0.5) * stepsize * 2);
        upper_edge = round((2^(no_bits-1) - 1.5) * stepsize * 2);
        partition = under_edge:(stepsize*2):upper_edge; 
        [~,delta_quantized] = quantiz(delta,partition,codebook);
        output(i) = delta_quantized;

        % Calculate delta prime
        delta_prime = round(delta_quantized * stepsize);
        delta_prime_array(i) = delta_prime;

        % Update the stepsize
        if i>10
            stepsize = StepsizeCalculation(delta_prime_array(i-9:i),no_bits);
            if stepsize == 0
                stepsize = 1;
            end  
        end
       
        % Calculate new prediction
        prediction = round((mu * (delta_prime + prediction))/2^15);
    end
end