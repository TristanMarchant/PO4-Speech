% main_fp.m file
% Calls the analysis, encoding, decoding and synthesis functions in this
% order with fixed point implementation

%TODO Maybe change comments to if statements with a paramater set at the
%beginning of the file

%% Get input from wav file
[input,fs,~,~] = LoadWav_new('m116');
left = input(1:2:end);
right = input(2:2:end);

%% Split input signal in subbands using analysis filter bank
[subbands, f0, f2] = splitSubbands_fp(left, fs);  
%f0 & f2 aren't scaled at this point(|f0(i)|< 1) will be in synthesis
close all
s0 = subbands(1,:);
%s0 = s0(1:(length(s0)/2));
s1 = subbands(2,:);
%s1 = s1(1:(length(s1)/2));
s2 = subbands(3,:);
%s2 = s2(1:(length(s2)/2));
s3 = subbands(4,:);
%s3 = s3(1:(length(s3)/2));
%s4 = subbands(5,:);
%s5 = subbands(6,:);
% s0, s1, s2, s4 scaled to (16,14)   
% Fixed parameters used in encoder and decoder
mu = [0.6645,0.2259,0.1576,-0.0958]; % Overall mu's
mu_fp = [21774, 7402, 5164, - 3139]; % overall rounded (16,15) mu's
%mu = [0.3112,-0.1919,-0.2557,-0.3919,0.1576,-0.0958]; % Left 6 subbands
bpsb = [4;4;2;2]; % bits per subband, sum should be 12 when 4 equal subbands
%bpsb = [5;4;4;3;2;2]; % for 6 subbands (left)

% %% Encode each subband
% [es0] = encode_fp(s0,mu_fp(1),bpsb(1));
% [es1] = encode_fp(s1,mu_fp(2),bpsb(2));
% [es2] = encode_fp(s2,mu_fp(3),bpsb(3));
% [es3] = encode_fp(s3,- mu_fp(4),bpsb(4));
% %[es4,delta_prime_array4,stepsize_array4,pred_arr4] = EncodeEllen(s4,mu(5),bpsb(5));
% %[es5,delta_prime_array5,stepsize_array5,pred_arr5] = EncodeEllen(s5,mu(6),bpsb(6));

% %% Decode each subband
% [ds0] = decode_fp(es0,mu_fp(1),bpsb(1));
% [ds1] = decode_fp(es1,mu_fp(2),bpsb(2));
% [ds2] = decode_fp(es2,mu_fp(3),bpsb(3));
% [ds3] = decode_fp(es3,- mu_fp(4),bpsb(4));
% %[ds4,delta_prime_array4x,stepsize_array4x,pred_arr4x] = DecodeEllen(es4,mu(5),bpsb(5));
% %[ds5,delta_prime_array5x,stepsize_array5x,pred_arr5x] = DecodeEllen(es5,mu(6),bpsb(6));

%% Combine the subbands using synthesis filter bank

y1 = synthesis_fp(s0,s1,f2); %f2 scaled within the function synthesis, s0 and s1 are scaled to (16,14), y1 to (16,13)
y2 = synthesis_fp(s2,s3,f2); %f2 scaled within the function synthesis, s2 and s3 are scaled to (16,14), y2 to (16,13)
result = synthesis_fp(y1,y2,f0); %f0 scaled within the function synthesis, result is scaled to (16,12)

[input_al, result_al] = alignsignals(left,result);
input_al = [input_al zeros(1,6)];
%% Show/play result
%plot both original and result
figure();
plot(input_al); hold on
plot(result_al);

title('original signal and processed signal');
legend('original signal', 'processed signal');

% calculate and display PESQ and SNR
pval = pesq(input_al',result_al',fs)
seg = snrseg(result_al',input_al',fs)

% play result
soundsc(result_al,fs);
