
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # #   Matlab Scripts Accompanying the Paper   # # # # # # # 
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # #
# # #  Elmar Rueckert and Andrea d'Avella.
# # #  Learned parametrized dynamic movement primitives with shared synergies for controlling robotic and musculoskeletal systems
# # #  in Frontiers in Computational Neuroscience.
# # #  Volumne 6, Year 2013, Number 97.
# # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

-= Content =-

  Overview
  Files
  Contact
  Warranty
  Software requirements
  Description of usage


-= Overview =-

The zip archive contains two Matlab scripts, a Matlab mex function that provides an interface to 
the OpenSimulator engine and a musculoskeletal model file (.osim)

The provided Matlab test file generates 11 muscle excitation patterns, 
which are simulated on an upper limb model of a human arm in OpenSim.

The result of this simulation is a marker trajectory which is plotted in Matlab. 

-= Files =-

The zip archive contains the following files:
compile.m                               compiles the Matlab mex function
run.m                                   runs the simulation and plots a two-dimensional marker trajectory
Rueckert2013UpperLimb11Muscles.osim     OpenSim model file of an upper limb model of a human arm 
license.txt                             Software license
readme.txt                              This document
simOsimModelLinearController.m          Header description of the mex function
simOsimModelLinearController.cpp        Mex function providing the interface to OpenSim

-= Contact =-

In case of questions, please contact

  Elmar Rueckert  (rueckert@igi.tugraz.at)


-= Warranty =-

The software is provided "as is", without warranty of any kind, express or implied,
including but not limited to the warranties of merchantability, fitness for a particular
purpose and non-infringement. In no event shall the authors or copyright holders be
liable for any claim, damages or other liability, whether in an action of contract,
tort or otherwise, arising from, out of or in connection with the software or the use
or other dealings in the software.


-= Software requirements =-

The scripts were tested on a system with the following software installed

  Fedora Linux
  Matlab 7.4.0.287 (R2007a)
  OpenSim 3.0.0


-= Description of usage =-

Extract the files to a directory and run first the "compile.m" script and afterwards the 
test function in "run.m".
After a couple of seconds a figure will pop up showing a marker trajectory. Other variables 
might also be recorded in the OpenSim simulation by adapting the code in "simOsimModelLinearController.cpp". 
For implementation details we refer to the OpenSim project page: http://www.simtk.org.


Version: 2013-08-28 by Elmar Rueckert

-= End of document =-
