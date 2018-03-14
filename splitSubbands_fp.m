% Function that splits the given input_signal in subbands
% input_signal: the signal to be split
% fs: sample frequency of the input_signal
% subbands: a matrix with different subband-signals in each row
% synthesis: a column vector with all synthesis_filters

function [subbands, f0, f2] = splitSubbands_fp(input_signal, fs)
%% QMF Design parameters
df = fs/10;
Astop = 30;
fstep = fs/800;
Niter = 100;
Flength1 = 32;
Flength2 = 16;
%Flength3 = 16;

%% Generate h and f filter impulse responses
% First tree stage filter
[h0,~,f0,~] = QMF_design(fs,df,Astop,fstep,Niter,Flength1);

% Second tree stage filter
[h2,~,f2,~] = QMF_design(fs/2,df/2,Astop,fstep/2,Niter,Flength2); 

% Third stage filter
%[h4,~,f4,~] = QMF_design(fs/4,df/4,Astop,fstep/4,Niter,Flength3); 

%% Analysis
[c0,c1] = analysis_fp(input_signal,h0); %h0 not scaled %1st argument should e scaled
[s0,s1] = analysis_fp(c0,h2);% c0 scaled 
[s2,s3] = analysis_fp(c1,h2);%c1 scaled
subbands = [s0;s1;s2;s3];
end