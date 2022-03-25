# Analytical-Models-for-NR-V2X-Mode-2-and-its-comaprison-to-C-V2X-Mode-4
 This repository is built to show the performance evaluation of both technologies NR-V2X mode 2 and C-V2X mode 4. The comparison is done based on the simulator and the  simulator and the proposed analytical models.
The analysis is performed in matlab. These models are designed and is the extension of work "M. Gonzalez-Martín, M. Sepulcre, R. Molina-Masegosa and J. Gozalvez, "Analytical Models of the Performance of C-V2X Mode 4 Vehicular Communications," in IEEE Transactions on Vehicular Technology, vol. 68, no. 2, pp. 1155-1166, Feb. 2019, doi: 10.1109/TVT.2018.2888704".
In this work we evalauted the performance of NR-V2X mode 2 with the re-evaluation mechanism as proposed by the #3GPP for the improved semi-persistent scheduling in NR-V2X mode 2.
It is observed that the NR-V2X perfroms better as compared to C-V2X beacuse of the NR numerologies. Due to this fact even in high density scenarios NR-V2X performs far better than the C-V2X .
Morover, the simulator for NR-V2X sub-6 GHz band is designed to evalaute the performance of the C-V2X and NR-V2X.

# NR-V2X-sub-6GHz-band Simulator


To use this first download the latest release of [ns-3](https://www.nsnam.org "ns-3 Website") module for the simulation.

This is ns-3 module for newradio as mentioned following
- inline with the 3gpp Rel 16, specifications
- this is soley built for research purpose. However, to include full features of 3gpp work is under consideration
- it includes full-stack operations including those from RLC and PDCP layers, thanks to the integration with the LTE module of ns-3
- this module studies the effect of NR-numerologies

## Getting Started ##

To use this module, you need to install [ns3]("https://www.nsnam.org "ns-3 Website"") and clone this repository inside the `src` folder:

## About ##

This module is being developed by [MONET Lab](http://monet.knu.ac.kr), [Kyungpook National University](https://knu.ac.kr).

## License ##

This software is licensed under the terms of the GNU GPLv2, as like as ns-3. See the LICENSE file for more details.
