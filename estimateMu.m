% Estimate mu such that the energy of the prediction error s(n)- ?*s(n-1)  
% is minimized
% input: signal for which mu should be estimated (row vector)
% output: the estimated value mu
function [mu] = estimateMu(signal)
    %TODO make sure signal is a row vector
    %TODO filter only speech out, omit silence parts (treshold)

    % define signal and delayed signal
    signal = signal(1, 2:length(signal));
    end_index = length(signal) - 1;
    signal_shifted = signal(1, 1:end_index);

    % solve a least square problem Ax=b equivalent to s_prev * mu = s_curr
    mu = signal'\signal_shifted';
end