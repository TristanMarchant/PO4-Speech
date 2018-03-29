% The following function determines the step size according to
% the quantized predicition error of the ADPCM encoder
% Inputs:   - input: The 10 previous samples of the quantized prediciton error to
%           calculate the standard deviation from
%           -bitspersample: The amount of bits per sample for the given
%           subband
% Output:    - The stepsize to use for the next sample

function [step_size] = StepsizeCalculation(input, bitspersample)

% The matrix stepsize_opt contains the various values of optimum phi's for
% uniform quantizers with various inputs
% Similar to the table 4.1, 
% Column (pdf) 1 : U & column 2 : G & column 3 : L & column 4 : Gamma
% The rows correspond to the number of bits per sample 

phi_init =     [1.7320 1.5956 1.4142 1.1547;
                0.8660 0.9957 1.0874 1.0660;
                0.4330 0.5860 0.7309 0.7957;
                0.2165 0.3352 0.4610 0.5400;
                0.1083 0.1881 0.2800 0.3459;
                0.0541 0.1041 0.1657 0.2130;
                0.0271 0.0569 0.0961 0.1273;
                0.0135 0.0308 0.0549 0.0743];
            
phi_init = round(phi_init*2^8);
Phi = phi_init(bitspersample,2) + round((0.3/bitspersample)*2^8); % the factor opt_ss/SD(output)
SD_input = round(std(input)*2^8); % standard deviation of the input
step_size = round((Phi * SD_input)/2^16);
end
            