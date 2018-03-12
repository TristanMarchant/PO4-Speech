no_bits = 4;
delta_array = -30:1:30;
stepsize = 4;
codebook = (-(2^no_bits)/2):1:((2^no_bits)/2-1);
output = zeros(1,length(delta_array));
delta_prime_array = zeros(1,length(delta_array));
%quantize delta
for i = 1:length(delta_array)
    delta = delta_array(i);
    under_edge = ( -(2^no_bits)/2+0.5 ) * stepsize;
    upper_edge = ( (2^no_bits)/2-1.5 ) * stepsize;
    partition = under_edge:stepsize:upper_edge; %edges of the intervals
    [~,delta_quantized] = quantiz(delta,partition,codebook);
    output(i) = delta_quantized;
    delta_prime = ( delta_quantized ) * stepsize;
    delta_prime_array(i) = delta_prime;
end
figure();
plot(delta_prime_array); hold on
plot(delta_array);