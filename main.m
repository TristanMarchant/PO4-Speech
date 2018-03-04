% Main .m file
%% Get input from wav file
[input,fs,~,~] = LoadWav_new('f116');
left = input(1:2:end);
right = input(2:2:end);

%% split left input signal in subbands using analysis filter bank
[subbands, f0, f2] = splitSubbands(left, fs);
close all
s0 = subbands(1,:);
s1 = subbands(2,:);
s2 = subbands(3,:);
s3 = subbands(4,:);

%% fixed parameters used in encoder and decoder
mu = [0.6054;0.3277;0.2760;-0.0738];
bpsb = [5;3;2;2]; %bits per subband, sum should be 12 when 4 subbands

%% Encode each subband
[es0] = EncodeEllen(s0,mu(1),bpsb(1));
[es1] = EncodeEllen(s1,mu(2),bpsb(2));
[es2] = EncodeEllen(s2,mu(3),bpsb(3));
[es3] = EncodeEllen(s3,mu(4),bpsb(4));

%% Decode each subband
ds0 = DecodeEllen(es0,mu(1),bpsb(1));
ds1 = DecodeEllen(es1,mu(2),bpsb(2));
ds2 = DecodeEllen(es2,mu(3),bpsb(3));
ds3 = DecodeEllen(es3,mu(4),bpsb(4));

%% combine the subbands using synthesis filter bank
%y3 = synthesis(s6,s7,f4);
%y2 = synthesis(s4,s5,f4);
y0 = synthesis(ds0,ds1,f2);
y1 = synthesis(ds2,ds3,f2);
result = synthesis(y0,y1,f0);

%% show/play result
%TODO Align result and input properly
result = [zeros(1,16) result];

% plot both original and result
figure();
plot(left); hold on
plot(result);
title('original signal and processed signal');
legend('original signal', 'processed signal');

% calculate and display PESQ
pval = pesq(left,result,8000);
disp("PESQ: " + pval);

% play result
soundsc(result,fs);

