%import the samples of a wav-file with the given filename
%using a sample frequency of 8kHz
%user example: test_data = importWaveFile('bir');
function [data] = importWaveFile(filename)
full_filename = fullfile('wavfiles', strcat(filename, '.wav'));
[data,Fs] = audioread(full_filename);

if Fs == 8000
    return;
elseif Fs == 16000
    data = downsample(data, 2);
else
    error("unable to down- or upsample to 8kHz");
end

end