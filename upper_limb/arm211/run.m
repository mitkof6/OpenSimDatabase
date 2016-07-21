
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

%% Description:
%The provided Matlab test file generates 11 muscle excitation patterns, 
%which are simulated on an upper limb model of a human arm in OpenSim.
%The result of this simulation is a marker trajectory which is plotted in Matlab. 

clear all;
close all;

Ts = 0.5; %simulation time in seconds
saveMotionFileSuffixId = 0; %set to values greater than 0 to save opensim files to the current directory
numMuscles = 11; %number of muscles in the osim model
osimFileName = 'Rueckert2013UpperLimb11Muscles.osim'; %this file should be in the current path or a full path is given
maxTimeStepsRecorded = 5000; %we use a buffer of fixed size to store 
%the simulation results. The buffer size should be large enough and depends 
%on the dynamics of the model (opensim uses variable integration times) and 
%the total simulation time. A warning will be prompted if the buffer was
%too small.
numOutputDimensions = 4; %can be set to 3 or 4. When set to 4, addionally the z-coordinates of the marker trajectory is returned


%% generate some muscle excitation patterns, the values should be between 0
%and 1
numDiscreteCommands = 2; %number of discrete commands that are linearly interpolated in [0 Ts]
fbounded = zeros(numMuscles,numDiscreteCommands);
fbounded(5,1) = 0.6;
fbounded(5,end) = 0.1;

%% run the simulation
EFtrajectory = simOsimModelLinearController(fbounded, Ts, ...
    maxTimeStepsRecorded, saveMotionFileSuffixId, osimFileName, numOutputDimensions); 

%read the results
markerTrajectory = EFtrajectory(1:numOutputDimensions-1,:);
time = EFtrajectory(end,:);
xCoordIndex = 1;
yCoordIndex = 2;
zCoordIndex = 3;

%% plot the marker trajectory
hfig = figure;
set(hfig,'Color','white');
plot(markerTrajectory(xCoordIndex,:), markerTrajectory(yCoordIndex,:), 'linewidth', 3);
set(gca,'fontsize', 24);
lH = legend('2D marker traj.');
set(lH, 'fontsize', 18);
xlabel('x coordinate [m]', 'fontsize', 24);
ylabel('y coordinate [m]', 'fontsize', 24);

hfig = figure; 
set(hfig,'Color','white');
plot(1:size(time,2), time, 'linewidth', 3);
set(gca,'fontsize', 24);
lH = legend('variable simulation time step');
set(lH, 'fontsize', 18);
xlabel('discrete time step', 'fontsize', 24);
ylabel('true time step [sec]', 'fontsize', 24);

