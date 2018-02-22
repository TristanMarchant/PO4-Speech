% Computes the synthesis of 2 subband signals
% Inputs: 2 subband signals
% Output: original singal

function [y] = synthesis(c0,c1,h0)

s0 = c0 + c1;
s1 = c0 - c1;

h0_even = h0(2:2:end);
h0_odd = h0(1:2:end);

f0 = fftfilt(s0,h0_odd);
f1 = fftfilt(s1,h0_even);

y = reshape([f0;f1], 1, []);

