% Main .m file
% Calls the analysis, encoding, decoding and synthesis functions in this
% order

%% Get input from wav file
[input,fs,~,~] = LoadWav_new('words_m');
left = input(1:2:end);
right = input(2:2:end);

%% Split input signal in subbands using analysis filter bank
[subbands, f0, f2] = splitSubbands(left, fs);
close all
s0 = subbands(1,:);
s1 = subbands(2,:);
s2 = subbands(3,:);
s3 = subbands(4,:);

%% Fixed parameters used in encoder and decoder
mu = [0.6645,0.2259,0.1576,-0.0958]; % Overall mu's
mu = round(mu * 2^15);
bpsb = [4;4;2;2]; % Bits per subband, sum should be 12 when 4 equal subbands

%% Encode each subband
[es0] = encode(s0,mu(1),bpsb(1));
[es1] = encode(s1,mu(2),bpsb(2));
[es2] = encode(s2,mu(3),bpsb(3));
[es3] = encode(s3,mu(4),bpsb(4));

%% Decode each subband
[ds0] = decode(es0,mu(1),bpsb(1));
[ds1] = decode(es1,mu(2),bpsb(2));
[ds2] = decode(es2,mu(3),bpsb(3));
[ds3] = decode(es3,mu(4),bpsb(4));

%% Combine the subbands using synthesis filter bank
y1 = synthesis(ds0,ds1,f2);
y2 = synthesis(ds2,ds3,f2);
result = synthesis(y1,y2,f0);
[input_al, result_al] = alignsignals(left,result);
input_al = [input_al zeros(1,6)]; % Add 6 samples for the PESQ calculation

%% Show/play result
% Plot both original and result
figure();
plot(input_al); hold on
plot(result_al);

title('original signal and processed signal');
legend('original signal', 'processed signal');

% Calculate and display PESQ and SNR
pesq_val = pesq(input_al',result_al',fs);
disp(pesq_val);
snrseg_val = snrseg(result_al',input_al',fs);
disp(snrseg_val);

% Play result
soundsc(result_al,fs);
