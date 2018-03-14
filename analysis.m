% Computes 2 subband signals from an input
% Inputs: inputstream and h0 (impulse response of the used filter)
% Outputs: 2 subband signals

function [c0,c1] = analysis(input,h0)

input_even = input(2:2:end);
input_odd = input(1:2:end);

h0_even = h0(2:2:end);
h0_odd = h0(1:2:end);
%Fixed point (no overflow abs(h0)<0.5)
h0_even = round(2^16*h0_even);
h0_odd = round(2^16*h0_odd);

x0 = conv(input_even,h0_even,'same');
y0 = conv(input_odd,h0_odd,'same');

x0 = round(x0/2^16);
y0 = round(y0/2^16);

% for i = 1:length(x0)
%     if x0(i) > 32767
%         error('error overflow');
%     end
% end

c0 = x0 + y0;
c1 = x0 - y0;