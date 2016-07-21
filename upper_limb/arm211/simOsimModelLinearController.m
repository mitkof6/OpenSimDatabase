function [output] = simOsimModelLinearController(controls, endTimeInSec, ...
    maxTimeStepsRecorded, saveMotionFileSuffixId, osimFileName, numOutputDimensions)
%# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
%# # # # # # # # #   Matlab Scripts Accompanying the Paper   # # # # # # # 
%# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
%# # #
%# # #  Elmar Rueckert and Andrea d'Avella.
%# # #  Learned parametrized dynamic movement primitives with shared synergies for controlling robotic and musculoskeletal systems
%# # #  in Frontiers in Computational Neuroscience.
%# # #  Volumne 6, Year 2013, Number 97.
%# # #
%# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
%# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
%
%This function provides an OpenSim Matlab interface to run forward dynamics
%simulations in musculoskeletal models. Given vectors of muscle excitation 
%patterns it returns a two or three-dimensional marker trajectory. 
%
%In the current version the marker name property in the osim file should 
%be set to "r_radius_styloid". Alternatively, the mex file can be adapted 
%to return different simulation quantities.
%
%Optional file header info
%
% Syntax:  [output] = function_name(controls, endTimeInSec, ...
%    maxTimeStepsRecorded, saveMotionFileSuffixId, osimFileName, numOutputDimensions)
%
% Inputs:
%    controls - [MxN] Matrix with muscle excitation values, where M denotes
%    the number of muscles and N the number of control values that are
%    between 0 and 1
%
%    endTimeInSec - Simulation time in seconds
%
%    maxTimeStepsRecorded - Buffer size for the results, something between
%    100 and 10000
%
%    saveMotionFileSuffixId - if > 0 output files with this suffix will be 
%    written that can be used in OpenSim to generate movies or screenshots
%
%    osimFileName - filename or filename and path to the osim model file
%
%    numOutputDimensions - can be 3 or 4. When set to 3, each column in the
%    output matrix will contain [x,y,time]'. When set to 4: [x,y,z,time]'.
%    
%
% Outputs:
%    output - [numOutputDimensions x T] Matrix, where T denotes the actual number of
%    simulation steps that depends on the simulation time and the dynamics
%    of the model as OpenSim uses variable integration step lenghts.
%
% Example: 
%   [output] = simOsimModelLinearController([0 0 0 0 0.5 0.5 0.2 0 0 0], 0.1, ...
%    500, 0, 'MyOsimModelWithOneMuscle.osim', 4);
%
% Other m-files required: none
% Subfunctions: none
% MAT-files required: none
%
% See also: www.simtk.org,  www.opensimulator.org
 
% Author: Elmar Rueckert
% Graz Institute of Technology, Austria
% email address: rueckert@igi.tugraz.at
% Oktober 2012; Last revision: 28-August-2013
