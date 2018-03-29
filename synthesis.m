% Computes the synthesis of 2 subband signals
% Inputs:   - 2 subband signals
%           - f: filter used for synthesis
% Output: original singal

function [y] = synthesis(c0,c1,f)

s0 = c0 + c1;
s1 = c0 - c1;

f_even = f(2:2:end);
f_odd = f(1:2:end);

f_even = round(2^15*f_even);
f_odd = round(2^15*f_odd);

f0 = conv(s0, f_odd,'same');
f1 = conv(s1, f_even,'same');

f0 = round(f0/2^15);
f1 = round(f1/2^15);

y = zeros(1,2*length(f0));
y(2:2:end) = f0;
y(1:2:end) = f1;

