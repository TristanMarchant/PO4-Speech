% Put the audio samples of a *.wav-file in a buffer with alternating L and R samples.
% Data is pairwise identical if mono input, interleaved if input is stereo.

% takes the filename without extention as input, should be placed in
% subfolder wavfiles
% x: row vector with interleaved left and right signal
%    Stereo : L1/R1/L2/R2/L3/R3/L4/R4/L5/R5/...
%    Mono -> duplicate the channel => L1/L1/L2/L2/L3/L3/L4/L4/L5/L5/...
% fs: sample frequency of both left and right signal
% nbits: number of bits per sample
% nr_channels: number of channels (1 for mono, 2 for stereo), if mono then
% left and right signal contain the same values
% author: Ellen
function [x, fs, nbits, nr_channels] = LoadWav_new(filename)

%% load data from file
full_filename = fullfile('wavfiles', strcat(filename, '.wav'));
[in,fs] = audioread(full_filename); %in will be a column-vector
wavinfo = audioinfo(full_filename);
nbits = wavinfo.BitsPerSample;
nr_channels = size(in,2);

%% downsample to 8kHz
if fs == 8000
elseif fs == 16000
    in = downsample(in, 2);
    fs = 8000;
else
    error('unable to downsample to 8kHz');
end

%% interleave both signals
if (nr_channels == 2) %stereo
    x = reshape(in.', 1, []); %x will be a row vector
else %mono
    temp = [in, in];
    x = reshape(temp.', 1, []); %x will be a row vector
end
end
