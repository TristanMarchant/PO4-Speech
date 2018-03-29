% Elementary block that computes 2 subband signals from an input
% Inputs:   -input: signal to be split 
%           -h: impulse response of the used filter
% Outputs:  -c0: lower frequency subband
%           -c1: higher frequency subband

function [c0,c1] = analysis(input,h)

input_even = input(2:2:end);
input_odd = input(1:2:end);

h_even = h(2:2:end);
h_odd = h(1:2:end);

h_even = round(2^15*h_even);
h_odd = round(2^15*h_odd);

x0 = conv(input_even,h_even,'same');
y0 = conv(input_odd,h_odd,'same');

x0 = round(x0/2^15);
y0 = round(y0/2^15);

c0 = x0 + y0;
c1 = x0 - y0;