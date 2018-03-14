% Computes the synthesis of 2 subband signals
% Inputs: 2 subband signals
% Output: original singal

function [y] = synthesis(c0,c1,f0)

s0 = c0 + c1;
s1 = c0 - c1;

h0_even = f0(2:2:end);
h0_odd = f0(1:2:end);

h0_even = round(2^16*h0_even);
h0_odd = round(2^16*h0_odd);

f0 = conv(s0, h0_odd,'same');
f1 = conv(s1, h0_even,'same');

f0 = round(f0/2^16);
f1 = round(f1/2^16);

% for i = 1:length(f0)
%     if f0(i) > 32767
%         error('error overflow');
%     end
% end

y = zeros(1,2*length(f0));
y(2:2:end) = f0;
y(1:2:end) = f1;

