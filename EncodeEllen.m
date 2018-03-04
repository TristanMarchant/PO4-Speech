function [output, delta_array, stepsize_array] = EncodeEllen(input,mu,no_bits)

%initialization
prediction = mu * input(1);
codebook = 0:1:(2^no_bits-1); %code to give to each interval
stepsize = 1;
output = zeros(1, length(input));
delta_array = zeros(1, length(input));
delta_prime_array = zeros(1, length(input));
stepsize_array = zeros(1, length(input));
stepsize_array(1) = stepsize;

for i = 2:length(input)
    delta = input(i) - mu * prediction;
    delta_array(i) = delta;

    %quantize delta
    under_edge = ( -(2^no_bits)/2+0.5 ) * stepsize;
    upper_edge = ( (2^no_bits)/2-1.5 ) * stepsize;
    partition = under_edge:stepsize:upper_edge; %edges of the intervals
    [~,delta_quantized] = quantiz(delta,partition,codebook);
    output(1,i) = delta_quantized;
    
    %calculate delta prime
    delta_prime = ( delta_quantized - 2^(no_bits-1) ) * stepsize;
    delta_prime_array(i) = delta_prime;
    
    % each 10 samples, update the stepsize
    if mod(i,10) == 0
        stepsize = StepsizeCalculation(delta_prime_array(i-9:i),no_bits);
        if stepsize == 0
            stepsize = 1;
        end  
    end
    stepsize_array(i) = stepsize;
    
    % calculate new prediction
    prediction = mu * (delta_prime + prediction);
end
end