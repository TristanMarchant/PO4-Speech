% Computes the synthesis of 2 subband signals
% Inputs: 2 subband signals
% Output: original singal

function [y] = synthesis_fp(c0,c1,f0) 

s0 = c0 + c1;
s1 = c0 - c1;

h0_even = round(2^15*f0(2:2:end));
h0_odd = round(2^15*f0(1:2:end));

f0 = round(conv(s0, h0_odd,'same')/2^16);
f1 = round(conv(s1, h0_even,'same')/2^16);

y = zeros(1,2*length(f0));
y(2:2:end) = f0;
y(1:2:end) = f1;

