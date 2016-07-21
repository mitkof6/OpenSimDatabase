%mex command to build the C files

flgUseStaticLibPaths = 1; %set this flag to 1 to use the rpath option which 
%was necessary to perform simulations on our matlab cluster 

if flgUseStaticLibPaths < 1
    %dynamic linking
    mexFile=' simOsimModelLinearController.cpp';
    if mislocked(mexFile)
        munlock(mexFile)
    end
    
    %get paths
    oSimDir = getenv('OPENSIM_INSTALL_DIR'); %ATTENTION: this enviroment variable needs to be defined
    oSimLibDir = sprintf(' \''%s/lib/lib', oSimDir);

    oSimIncludeDir1 = [oSimDir '/sdk/include'];
    oSimIncludeDir2 = [oSimDir '/sdk/include/SimTK/include'];
    include = sprintf(' -I\''%s\'' -I\''%s\''',oSimIncludeDir1, oSimIncludeDir2);

    %set shared libs that are used
    libs=[oSimLibDir 'osimSimulation.so'''...
          oSimLibDir 'osimCommon.so'''...
          oSimLibDir 'osimActuators.so'''...
          oSimLibDir 'osimTools.so'''...
          oSimLibDir 'osimAnalyses.so'''...
          oSimLibDir 'SimTKcommon.so'''...
          oSimLibDir 'SimTKsimbody.so'''...
          oSimLibDir 'SimTKmath.so'''...
          oSimLibDir 'SimTKlapack.so'''];

    eval(['mex -v ' mexFile libs include]);

    disp('compilation done!'); 

else
    %static linking
    
    %get paths
    oSimDir = getenv('OPENSIM_INSTALL_DIR'); %this enviroment variable needs to be defined
    oSimLibDir = ' -l';

    oSimIncludeDir1 = [oSimDir '/sdk/include'];
    oSimIncludeDir2 = [oSimDir '/sdk/include/SimTK/include'];
    include = sprintf(' -I\''%s\'' -I\''%s\''',oSimIncludeDir1, oSimIncludeDir2);

    %set shared libs that are used
    libs=[oSimLibDir 'osimSimulation'...
          oSimLibDir 'osimCommon'...
          oSimLibDir 'osimActuators'...
          oSimLibDir 'osimTools'...
          oSimLibDir 'osimAnalyses'...
          oSimLibDir 'SimTKcommon'...
          oSimLibDir 'SimTKsimbody'...
          oSimLibDir 'SimTKmath'...
          oSimLibDir 'SimTKlapack'];

    mexFile=' simOsimModelLinearController.cpp';

    ldflags = sprintf('LDFLAGS=''\$LDFLAGS -L%s/lib %s''',oSimDir, libs);

    if mislocked(mexFile)
        munlock(mexFile)
    end

    %Note we use the rpath option to run the mex function also on our clusters, were dynamic linking 
    %in Matlab did not work 
    %Thus, you will have to change the hard coded paths in the lines below:
    mex -v CC="gcc" CFLAGS="\$CFLAGS" LDFLAGS="\$LDFLAGS -Wl','-rpath','/home/mammoth/elmar/lehre/FrameWorks/MotorControlFramework/Simulator/simtk/opensimbin/lib" ...
        -I/home/mammoth/elmar/lehre/FrameWorks/MotorControlFramework/Simulator/simtk/opensimbin/sdk/include ...
        -I/home/mammoth/elmar/lehre/FrameWorks/MotorControlFramework/Simulator/simtk/opensimbin/sdk/include/SimTK/include ...
        -L/home/mammoth/elmar/lehre/FrameWorks/MotorControlFramework/Simulator/simtk/opensimbin/lib ...
        -losimSimulation -losimCommon -losimActuators -losimTools -losimAnalyses -lSimTKcommon -lSimTKsimbody -lSimTKmath -lSimTKlapack ...
        simOsimModelLinearController.cpp

    disp('compilation done!'); 
end