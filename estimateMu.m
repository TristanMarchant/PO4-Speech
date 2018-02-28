% Estimate mu such that the energy of the prediction error s(n)- mu*s(n-1)  
% is minimized
% nb_subbands: number of subbands
% mu: a column-vector with the estimated values of mu per subband
% author: Ellen
function [mu] = estimateMu(nb_subbands)
    training_data = ["belasting", "bir", "f116", ... "f216", 
        "m116", "m216", ...
        "words_f", "words_m"];
    %calculate mu based on training data of different files
    mu_estimations = zeros(nb_subbands, size(training_data,2));
    for j = 1:size(training_data,2)
        [input,fs,~,~] = LoadWav_new(char(training_data(1,j)));
        left = input(1:2:end);
        % right = input(2:2:end);
        [subbands,~,~] = splitSubbands(left, fs);
        close ALL;
        
        mu_per_subband = zeros(size(subbands,1),1);
        %process each subband
        for i = 1:size(subbands,1)
            subband_signal = subbands(i,:);
            figure
            plot(subband_signal);
            title("original signal");
            mu_per_subband(i,1) = estimateMuPerSubband(subband_signal);
        end
        mu_estimations(:,j) = mu_per_subband;
    end
    
    %calculate mean of all mu estimations of one subband
    mu = zeros(nb_subbands,1);
    for k = 1:nb_subbands
        mu(k,1) = mean(mu_estimations(k,:));
    end
end

%estimate mu given a subband signal by minimizing s_curr - mu * s_prev
% signal: subband signal to process
% mu: the result of the estimation
%author: Ellen
function [mu] = estimateMuPerSubband(signal)
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
    % TODO PESQ?
    mu = signal_unshifted'\signal_shifted';
end