%The following function determines the step size according to the output of
%the ADPCM encoder

function [step_size] = StepsizeCalculation(input, bitspersample)

%The adaptive quantizer will be assumed to have a uniform characteristic
%unless otherwise mentionned
%the vector ss_opt contains the various values of optimum step sizes for
%uniform quantizers with various inputs
%Similar to the table 4.1, 
%column 1 : U & column 2 : G & column 3 : L & column 4 : Gamma
%the rows correspond to the number of bits per sample 

stepsize_opt = [1.7320 1.5956 1.4142 1.1547;
                0.8660 0.9957 1.0874 1.0660;
                0.4330 0.5860 0.7309 0.7957;
                0.2165 0.3352 0.4610 0.5400;
                0.1083 0.1881 0.2800 0.3459;
                0.0541 0.1041 0.1657 0.2130;
                0.0271 0.0569 0.0961 0.1273;
                0.0135 0.0308 0.0549 0.0743];
            
Phi = stepsize_opt(bitspersample,2)+0.3/bitspersample; %the factor opt_ss/var(output)
SD_input = std(input); % standard deviation of the input
step_size = Phi * SD_input;

end
            