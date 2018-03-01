% Main .m file

%% QMF Design parameters
fs = 8000;
df = fs/10;
Astop = 60;
fstep = fs/800;
Niter = 100;
Flength1 = 64;
Flength2 = 32;
%Flength3 = 32;

%% Generate h0, h1, f0 and f1
[h0,h1,f0,f1] = QMF_design(fs,df,Astop,fstep,Niter,Flength1); % First tree stage filter
[h2,h3,f2,f3] = QMF_design(fs/2,df/2,Astop,fstep/2,Niter,Flength2); % Second tree stage filter
%[h4,h5,f4,f5] = QMF_design(fs/4,df/4,Astop,fstep/4,Niter,Flength3); % Third stage filter

%% Get input from wav file
[input,fs,~,~] = LoadWav_new('f116');
left = input(1:2:end);
left = left*2^15;
right = input(2:2:end);

%% Analysis
[c0,c1] = analysis(left,h0);
[s0,s1] = analysis(c0,h2);
[s2,s3] = analysis(c1,h2);
%[s4,s5] = analysis(s0,h4);
%[s6,s7] = analysis(s1,h4);

%% Encode
es0 = Encode(s0,0.60);
es1 = Encode(s1,0.33);
es2 = Encode(s2,0.28);
es3 = Encode(s3,-0.07);

%% Decode
ds0 = decode(es0,0.60);
ds1 = decode(es1,0.33);
ds2 = decode(es2,0.28);
ds3 = decode(es3,-0.07);

%% Synthesis
%y3 = synthesis(s6,s7,f4);
%y2 = synthesis(s4,s5,f4);
y0 = synthesis(ds0,ds1,f2);
y1 = synthesis(ds2,ds3,f2);
result = synthesis(y0,y1,f0);

%% Sound
figure();
plot(left); hold on
plot(result);
pval = pesq(left,result,8000)
soundsc(result,fs);

