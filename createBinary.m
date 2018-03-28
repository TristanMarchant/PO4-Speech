% function to create a binary file containing matlab data which will
% be read in the C code to test and debug
function [] = createBinary(inputArray)
fid = fopen('xbinary.bin','w');
fwrite(fid, inputArray, 'int');
fclose(fid);
end