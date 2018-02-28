%%ADPCM encoder function
function [enc] = ADPCMEncoder(in)

% enc refers to the encoded signal z(n)
% step_size should not be dependent on d since it won't be given later
% z resulting from the quantization of d
% step_size(1,1) = ss_ini to be definded for each subband
% mu is fixed per frequency band
%TODO write a function [ss] = StepsizeCalculation(z) that goes through the cases of
%z(1,i) to determine the new stepsize



%%initialization
    mu = 6;
    ss_ini = 16 % the step size ss(n) is set to the minimum value (16) 
    z = zeros(1, length(in));
    s_pred = zeros(1, length(in)-1); %s*, predicting s(n)
    %s_app approximates 

%%ADPCM encoder design
for i = [2:length(in)] % or chosen number of samples per band
    z(1,i) = round(delta /step_size); 
    s_pred(1,i) = mu * s_app;
    delta = in(1,i) - s_pred(1,i-1); 
    step_size = StepsizeCalculation(z(1,i));
    delta_deq = step_size*z(1,i); 
    S1(1,i) = mu*in(1,i-1)
    
end    
    
    
   
    

end