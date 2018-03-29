% Function that splits the given input signal in  4 subbands
% input_signal: the signal to be split
% fs: sample frequency of the input_signal
% subbands: a matrix with different subband-signals in each row
% f0 and f2, filter impulse responses used in synthesis

function [subbands, f0, f2] = splitSubbands(input_signal, fs)
%% QMF Design parameters
df = fs/10;
Astop = 30;
fstep = fs/800;
Niter = 100;
Flength1 = 32;
Flength2 = 16;

%% Generate h and f filter impulse responses
% First tree stage filter
[h0,~,f0,~] = QMF_design(fs,df,Astop,fstep,Niter,Flength1);

% Second tree stage filter
[h2,~,f2,~] = QMF_design(fs/2,df/2,Astop,fstep/2,Niter,Flength2); 

%% Analysis
[c0,c1] = analysis(input_signal,h0);
[s0,s1] = analysis(c0,h2);
[s2,s3] = analysis(c1,h2);
subbands = [s0;s1;s2;s3];

end