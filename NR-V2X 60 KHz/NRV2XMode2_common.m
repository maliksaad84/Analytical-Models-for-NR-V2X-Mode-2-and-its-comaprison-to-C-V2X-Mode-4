function [ deltaHD , deltaSEN , deltaPRO ] = NRV2XMode2_common( lambda , Pt , distance, Psen , step_dB , noise , coding);

% NRV2XMode2_common is a script that calculates the probability of packet 
% loss due to half-duplex transmissions, the probability of packet loss 
% due to a received signal power below the sensing power threshold and the
% probability of packet loss due to propagation effects for different 
% Tx-Rx distances 
%
% Input parameters:
%   lambda: packet transmission frequency in Hz. .
%   Pt: transmission power in dBm. 
%   distance: distance between tramsmitter and receiver in meters. It can be a vector with multiple distances.
%   Psen: sensing threshold in dBm.
%   step_dB: discrete steps to compute the PDF of the SNR and SINR in dB.
%   noise: noise corresponding to the DATA field of each message. Assumes a noise figure of 9dB and 10MHz channel (background noise of -95dBm). The total number of RBs in 10MHz is 50.
%   coding: ID of the coding used to identify the BLER curve 
% 
% Output metrics:
%    deltaHD: probability of packet loss due to half-duplex transmissions for different Tx-Rx distances
%    deltaSEN: probability of packet loss due to a received signal power below the sensing power threshold for different Tx-Rx distances
%    deltaPRO: probability of packet loss due to propagation effects for different Tx-Rx distances


  
    D = length(distance);

    deltaHD(1:D) = lambda/4000;  %NR-V2X with 60 kHz subcarrier spacing

    [PL_E_R, std_dev_E_R] = get_PL_SH(distance);   % Obtains pathloss and shadowing for different Tx-Rx distances.
    deltaSEN = 0.5 * ( 1 - erf( (Pt - PL_E_R - Psen)./(std_dev_E_R*sqrt(2)) ) );         

    [SNR, PDF_SNR] = get_SINRdistribution( Pt - PL_E_R , -180 , std_dev_E_R , 3 , noise , Psen , step_dB); % Obtains the PDF of the SNR experienced by Rx (without interference).
    deltaPRO = get_BLER( SNR , PDF_SNR , coding , step_dB); 

end



