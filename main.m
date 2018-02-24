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
right = input(2:2:end);

%% Analysis
[c0,c1] = analysis(left,h0);
[s0,s1] = analysis(c0,h2);
[s2,s3] = analysis(c1,h2);
%[s4,s5] = analysis(s0,h4);
%[s6,s7] = analysis(s1,h4);


%% BETWEEN ANALYSIS AND SYNTHESIS WILL BE PROCESSING

%% Synthesis
%y3 = synthesis(s6,s7,f4);
%y2 = synthesis(s4,s5,f4);
y0 = synthesis(s0,s1,f2);
y1 = synthesis(s2,s3,f2);
result = synthesis(y0,y1,f0);

%% Sound
figure();
plot(left); hold on
plot(result);
soundsc(result,fs);

