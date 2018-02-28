% Estimate mu such that the energy of the prediction error s(n)- mu*s(n-1)  
% is minimized
% input: signal for which mu should be estimated (row vector)
% output: the estimated value mu
function [mu] = estimateMu(signal)
    %make sure signal is a row vector
    if size(signal,1) > size(signal, 2)
        signal = transpose(signal);
    end
    
    %TODO define treshold
    treshold = max(signal)/100;
    
    %filter only speech out, omit silence parts at beginning and end 
    %(based on treshold)
    start_i = 0;
    end_i = 0;
    for i = 1:length(signal)
        %signal gets above treshold (from beginning) and is not set yet
        if abs(signal(1,i)) > treshold && start_i == 0
            start_i = i;
        end
        j = length(signal)-i+1;
        %signal gets above treshold (from end) and is not set yet
        if abs(signal(1,j)) > treshold && end_i == 0
            end_i = j;
        end
        %stop scanning when both i's are set
        if start_i ~= 0 && end_i ~= 0
            break;
        end
    end
    %error while scanning for sound
    if start_i == 0 || end_i == 0
        error('no signal could be found');
    end
    
    % define signal and delayed signal
    signal_unshifted = signal( 1, (start_i+1):end_i );
    signal_shifted = signal( 1, start_i:(end_i-1) );

    % solve a least square problem Ax=b equivalent to s_prev * mu = s_curr
    mu = signal_unshifted'\signal_shifted';
end