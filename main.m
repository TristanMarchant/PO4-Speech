% Main .m file
% Calls the analysis, encoding, decoding and synthesis functions in this
% order

%TODO Maybe change comments to if statements with a paramater set at the
%beginning of the file

%% Get input from wav file
[input,fs,~,~] = LoadWav_new('f116');
left = input(1:2:end);
right = input(2:2:end);

%% Split input signal in subbands using analysis filter bank
[subbands, f0, f2] = splitSubbands(left, fs);
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

%% Fixed parameters used in encoder and decoder
mu = [0.6645,0.2259,0.1576,-0.0958]; % Overall mu's
mu = round(mu * 2^15);
%mu = [0.3112,-0.1919,-0.2557,-0.3919,0.1576,-0.0958]; % Left 6 subbands
bpsb = [4;4;2;2]; % bits per subband, sum should be 12 when 4 equal subbands
%bpsb = [5;4;4;3;2;2]; % for 6 subbands (left)

%% Encode each subband
[es0] = encode(s0,mu(1),bpsb(1));
[es1] = encode(s1,mu(2),bpsb(2));
[es2] = encode(s2,mu(3),bpsb(3));
[es3] = encode(s3,mu(4),bpsb(4));
%[es4,delta_prime_array4,stepsize_array4,pred_arr4] = EncodeEllen(s4,mu(5),bpsb(5));
%[es5,delta_prime_array5,stepsize_array5,pred_arr5] = EncodeEllen(s5,mu(6),bpsb(6));

%% Decode each subband
[ds0] = decode(es0,mu(1),bpsb(1));
[ds1] = decode(es1,mu(2),bpsb(2));
[ds2] = decode(es2,mu(3),bpsb(3));
[ds3] = decode(es3,mu(4),bpsb(4));
%[ds4,delta_prime_array4x,stepsize_array4x,pred_arr4x] = DecodeEllen(es4,mu(5),bpsb(5));
%[ds5,delta_prime_array5x,stepsize_array5x,pred_arr5x] = DecodeEllen(es5,mu(6),bpsb(6));

%% Combine the subbands using synthesis filter bank
%y4 = synthesis(ds4,ds5,f2);
%y3 = synthesis(ds2,ds3,f4);
%y2 = synthesis(ds0,ds1,f4);
%y1 = synthesis(y2,y3,f2);
%result = synthesis(y1,y4,f0);
y1 = synthesis(ds0,ds1,f2);
y2 = synthesis(ds2,ds3,f2);
result = synthesis(y1,y2,f0);
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
