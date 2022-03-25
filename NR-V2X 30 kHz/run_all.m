function run_all

% NRV2XMode2 is the main script to evaluate the models and the input parameters are:
%    beta: traffic density in veh/m, e.g. 0.1.
%    lambda: packet transmission frequency in Hz, e.g. 10.
%    Pt: transmission power in dBm, e.g. 20.
%    S: number of sub-channels, e.g. 2.
%    B: packet size in bytes, e.g. 190.
   
%    NRV2XMode2(beta,lambda,Pt,S,B);

    NRV2XMode2(0.1,10,20,4,190);
   
  %  NRV2XMode2(0.2,10,20,4,190);
   % NRV2XMode2(0.3,10,20,4,190);
    
   % NRV2XMode2(0.1,10,23,4,190);
   % NRV2XMode2(0.2,10,23,4,190);
   % NRV2XMode2(0.3,10,23,4,190);
    
   % NRV2XMode2(0.1,20,20,4,190);
   % NRV2XMode2(0.2,20,20,4,190);
   % NRV2XMode2(0.3,20,20,4,190);
    
   % NRV2XMode2(0.1,10,20,2,190);
   % NRV2XMode2(0.2,10,20,2,190);
   % NRV2XMode2(0.3,10,20,2,190);


return
