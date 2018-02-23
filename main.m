% Main .m file

%% QMF Design parameters
fs = 8000;
df = fs/10;
Astop = 60;
fstep = fs/4410;
Niter = 100;
Flength1 = 0;
Flength2 = 0;

%% Generate h0, h1, f0 and f1
[h0,h1,f0,f1] = QMF_design(fs,df,Astop,fstep,Niter,Flength1); % First tree stage filter
[h2,h3,f2,f3] = QMF_design(fs,df,Astop,fstep,Niter,Flength2); % Second tree stage filter

%% Get input from wav file
[input,~,~,~] = LoadWav_new('f116');
left = input(1:2:end);
right = input(2:2:end);

%% Analysis
[c0,c1] = analysis(left,h0);
[s0,s1] = analysis(c0,h2);
[s2,s3] = analysis(c1,h2);

%% BETWEEN ANALYSIS AND SYNTHESIS WILL BE PROCESSING

%% Synthesis
y0 = synthesis(s0,s1,f2);
y1 = synthesis(s2,s3,f2);
result = synthesis(y0,y1,f0);

%% Sound
figure();
plot(left); hold on
plot(result);
soundsc(result,fs);

