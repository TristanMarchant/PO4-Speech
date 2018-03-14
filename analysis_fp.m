% Computes 2 subband signals from an input
% Inputs: inputstream and h0 (impulse response of the used filter)
% arg input are scaled to (16,15)
% Outputs: 2 subband signals scaled to (16,14)

function [c0,c1] = analysis_fp(input,h0) 

input_even = input(2:2:end); 
input_odd = input(1:2:end);

%scaling and selecting odd/even coefficients
h0_even = round(2^15*h0(2:2:end));
h0_odd = round(2^15*h0(1:2:end));

x0 = round(conv(input_even,h0_even,'same')/2^16);% (16,15)*(16.15) = (32,30) => (16,14)
y0 = round(conv(input_odd,h0_odd,'same')/2^16);

c0 = x0 + y0;
c1 = x0 - y0;