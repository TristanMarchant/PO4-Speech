% Main .m file
%% Get input from wav file
[input,fs,~,~] = LoadWav_new('f116');
left = input(1:2:end);
right = input(2:2:end);

[subbands, f0, f2] = splitSubbands(left, fs);
close all
s0 = subbands(1,:);
s1 = subbands(2,:);
s2 = subbands(3,:);
s3 = subbands(4,:);

%% Encode
[es0,stepsizes0] = Encode(s0,0.30,5);
[es1,stepsizes1] = Encode(s1,0.30,3);
[es2,stepsizes2] = Encode(s2,0.20,2);
[es3,stepsizes3] = Encode(s3,0.2,2);
% figure();
% plot(es0); hold on
% plot(stepsizes0)

%% Decode
[ds0,stepsizer] = decode(es0,0.30,5);
ds1 = decode(es1,0.30,3);
ds2 = decode(es2,0.20,2);
ds3 = decode(es3,0.2,2);
% figure();
% stepsizes0 = [stepsizes0,0];
% plot(stepsizer-stepsizes0);
%% Synthesis
%y3 = synthesis(s6,s7,f4);
%y2 = synthesis(s4,s5,f4);
y0 = synthesis(ds0,ds1,f2);
y1 = synthesis(ds2,ds3,f2);
result = synthesis(y0,y1,f0);

%% Sound
figure();
%TODO Align result and input
result = [zeros(1,16) result];
plot(left); hold on
plot(result);
pval = pesq(left,result,8000)
soundsc(result,fs);

