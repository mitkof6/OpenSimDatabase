

#include "math.h"
#include "mex.h"  
#include <OpenSim/OpenSim.h>

using namespace OpenSim;
using namespace SimTK;
using namespace std;


/**
 * A class for recording the endeffector of a model
 * during a simulation.
 *
 * @author Elmar Rueckert
 * @version 1.0
 */
class EFReporter : public Analysis {
OpenSim_DECLARE_CONCRETE_OBJECT(EFReporter, Analysis);

//=============================================================================
// DATA
//=============================================================================
private:
	Model* _model;
    int _numOutputDimensions;
    
protected:

//=============================================================================
// METHODS
//=============================================================================
public:
	EFReporter(Model *aModel, int maxTimeSteps, int numOutputDimensions) : Analysis(aModel)
	{
		_model = aModel;
        _maxTimeSteps = maxTimeSteps;
        _absPositions = mxCreateDoubleMatrix(numOutputDimensions, _maxTimeSteps, mxREAL);
        _mxEFPositionsPointer = mxGetPr(_absPositions);
        setName("EndEffectorAnalysis");
        _numOutputDimensions = numOutputDimensions;
	}
    
    ~EFReporter()
    {
        //delete all allocated vectors
        mxDestroyArray(_absPositions);
    }

    /**
     *returns the number of simulations steps (time-step varying integrator)
     */
    int getNumStepsSimulated()
    {
        return _trueNumSteps;
    }
    
    /**
     *Saves the stored endeffector positions into a "pre-allocated" matlab mex array
     */
    void fillEFPositionsMatrix(double* ptrOut)
    {
        memcpy(ptrOut, _mxEFPositionsPointer, sizeof(double)*_trueNumSteps*_numOutputDimensions);
    }
    
	//--------------------------------------------------------------------------
	// ANALYSIS
	//--------------------------------------------------------------------------
#ifndef SWIG

    virtual int
        step(const SimTK::State& s, int setNumber )
		{
            if(setNumber > _maxTimeSteps-1)
            {
                printf("warning setNumber > maxTimeStepsBuffer\n");
                return (0);
            }
            const MarkerSet& ms = _model->getMarkerSet();
            const Marker& m = ms.get("r_radius_styloid");
            m.getOffset(_relPosition);
            const SimbodyEngine& engine = _model->getSimbodyEngine();
            engine.transformPosition(s, m.getBody(), _relPosition, _model->getGroundBody(), _absPosition);
            
            //to read the marker offsets
            engine.transformPosition(s, m.getBody(), _relPosition, m.getBody(), _relPosition);
            
            int id = _numOutputDimensions*setNumber;
            if(id + _numOutputDimensions - 1 < _maxTimeSteps -1)
            {
                if(_numOutputDimensions < 4) //x,y,time
                {
                    _mxEFPositionsPointer[id + 0] = _absPosition(0);
                    _mxEFPositionsPointer[id + 1] = _absPosition(1);
                    _mxEFPositionsPointer[id + 2] = s.getTime();
                }
                else if(_numOutputDimensions < 5)//x,y,z,time
                {
                    _mxEFPositionsPointer[id + 0] = _absPosition(0);
                    _mxEFPositionsPointer[id + 1] = _absPosition(1);
                    _mxEFPositionsPointer[id + 2] = _absPosition(2);
                    _mxEFPositionsPointer[id + 3] = s.getTime();
                } 
                else//x,y,z, velProfile, time
                {
                    _mxEFPositionsPointer[id + 0] = _absPosition(0);
                    _mxEFPositionsPointer[id + 1] = _absPosition(1);
                    _mxEFPositionsPointer[id + 2] = _absPosition(2);
                    //TODO find out how to compute this
                    _mxEFPositionsPointer[id + 3] = -999.9;//_relPosition(0);//sqrt(_relPosition(0)*_relPosition(0) 
                        //+ _relPosition(1)*_relPosition(1) 
                        //+ _relPosition(2)*_relPosition(2));
                    _mxEFPositionsPointer[id + 4] = s.getTime();
                } 
                _trueNumSteps = setNumber;
            }
            
            
			return (0);
		}

#endif
private:
	mxArray* _absPositions;
    Vec3 _massCenter;
    Vec3 _relPosition;
    Vec3 _absPosition;
    int _maxTimeSteps;
    int _trueNumSteps;
    double* _mxEFPositionsPointer;
//=============================================================================
};	

class MyController : public ControlSetController {
OpenSim_DECLARE_CONCRETE_OBJECT(MyController, ControlSetController);

// This section contains methods that can be called in this controller class.
public:
	/**
	 * Constructor
	 *
	 * @param aModel Model to be controlled
	 * @param aKp Position gain by which the position error will be multiplied
	 */
	MyController(Model *aModel, const mxArray* muscleActivations, double endTime) : ControlSetController(), 
        _model(aModel), _muscleActivations(muscleActivations), _endTime(endTime)
	{
        _numMuscleActivations = mxGetN(_muscleActivations); //number of columns
        _numMuscles =  mxGetM(_muscleActivations);//numRows
        _mxControlPointer = mxGetPr(_muscleActivations);
        
        _dt = endTime / ((double)_numMuscleActivations);
        
        setName("MuscleSynergyController");
        
        //check if the provided number control trajectories are equal to the number of muscles in the model
        if(_numMuscles != _model->getMuscles().getSize())
            throw "number of control trajectories are not equal to the number of muscles in the model!";
        
        //create a controlset
         _muscleControls = new ControlSet();
         
         //specify the controls
         readInControls();
         
         //publish the controlset
         setControlSet(_muscleControls);
	}

    ~MyController()
    {
        //delete all allocated vectors
        for(int i=0; i < _allocatedControlLinearNode.size(); i++)
        {
            if(_allocatedControlLinearNode[i] != NULL)
            {
                delete _allocatedControlLinearNode[i];
                _allocatedControlLinearNode[i] = NULL;
            }
        }
    }
    
    void  readInControls()
    {
        double t, val;
        int id;
        
        //step through all muscles
        for(int mId = 0; mId < _numMuscles; mId++)
        {
           Muscle& mTmp = _model->getMuscles()[mId];
           
           ControlLinear *control_M = new ControlLinear();
           //cout << mTmp.getName() << endl;
           control_M->setName(mTmp.getName());
           control_M->setUseSteps(false);
           control_M->setKp(100);
           control_M->setKv(20);
           
           //step through all control signals
           for(int cId=0; cId < _numMuscleActivations; cId++)
           {
               id = cId*_numMuscles + mId;
               t = 1.0e-6 + cId*_dt;
               val = _mxControlPointer[id];
               ControlLinearNode *cn = new ControlLinearNode(t, val);
               
               _allocatedControlLinearNode.push_back(cn);
               control_M->insertNewValueNode(cId, *cn);
           }
           
           //min max nodes (optional)
           ControlLinearNode *cnMin1 = new ControlLinearNode(0.0, 0.0);
           ControlLinearNode *cnMin2 = new ControlLinearNode(_endTime, 0.0);
           ControlLinearNode *cnMax1 = new ControlLinearNode(0.0, 1.0);
           ControlLinearNode *cnMax2 = new ControlLinearNode(_endTime, 1.0);
           _allocatedControlLinearNode.push_back(cnMin1);
           _allocatedControlLinearNode.push_back(cnMin2);
           _allocatedControlLinearNode.push_back(cnMax1);
           _allocatedControlLinearNode.push_back(cnMax2);
           control_M->insertNewMinNode(0, *cnMin1);
           control_M->insertNewMinNode(1, *cnMin2);
           control_M->insertNewMaxNode(0, *cnMax1);
           control_M->insertNewMaxNode(1, *cnMax2);
           
           _muscleControls->adoptAndAppend(control_M);
        }
    }   


// This section contains the member variables of this controller class.
private:
    //Normally the model should free all allocated memory
    vector<ControlLinearNode*> _allocatedControlLinearNode;
    ControlSet* _muscleControls;
    
    Model* _model;
    const mxArray* _muscleActivations;
    double* _mxControlPointer;
    double _endTime;
    int _numMuscleActivations;
    int _numMuscles;
    double _dt;
};



/* Multi-DoF Osim Model using an arbitrary number of Muscles */
/* adapted by Elmar Rueckert 2013 */

/* endeffectorTrajectory(4-dimensional:x,y,z,time) = 
 simOsimModelLinearController(controls, endTimeInSec, maxTimeStepsRecorded, 
 saveMotionFileSuffixId, osimFileName, numOutputDimensions) */

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
	mexLock();
    //read inputs (index 0 are the muscle activations)
    double* finalTime_ptr = mxGetPr(prhs[1]);
    double* maxTimeSteps_ptr = mxGetPr(prhs[2]);
    double* numOutputDimensions_ptr = mxGetPr(prhs[5]);
    double initialTime = 0;
    double finalTime = finalTime_ptr[0];
    double intAccuracy = 1.0e-3;
    double intMinStepSize = 5.0e-4;
    int maxTimeSteps = (int)maxTimeSteps_ptr[0];
    int numOutputDimensions = (int)numOutputDimensions_ptr[0];
    if(numOutputDimensions < 3 || numOutputDimensions > 5)
        numOutputDimensions = 3;
    //Default: x,y, time
    //_numOutputDimensions = 3;
    //x, y, z, time
    //_numOutputDimensions = 4;
    //x, y, z, velProfile, time
    //_numOutputDimensions = 5;
        
    
    
    //*******************************
    //Load the model
    //opensim V3.0 model
    char* tmp = mxArrayToString(prhs[4]);
    std::string osimFileName = tmp;
    //printf("OsimFileName: %s \n",osimFileName);
    Model osimModel(osimFileName);
    
    //*******************************
    // Create a linear controller with given controlnodes.
    MyController *controller = new MyController(&osimModel, prhs[0], finalTime);
    controller->setActuators( osimModel.updActuators() );
    osimModel.addController( controller );
    
    //*******************************
    // Create Analyzer to store the end effector coords.
    EFReporter* reporterEF = new EFReporter(&osimModel, maxTimeSteps, numOutputDimensions);
    osimModel.addAnalysis(reporterEF);
    
    
    //*******************************
    //set up the state (initial joint angles, etc)
    State& si = osimModel.initSystem();
    osimModel.equilibrateMuscles(si);
    
    //******************************
    //Setup the simulator and integrator
    RungeKuttaMersonIntegrator integrator(osimModel.getMultibodySystem());
    integrator.setAccuracy(1.0e-4);
    
    Manager manager(osimModel, integrator);
    manager.setInitialTime(initialTime);
    manager.setFinalTime(finalTime);
    osimModel.getMultibodySystem().realize(si, Stage::Velocity);

    //******************************
    //Run the simulation
    manager.integrate(si);

    //******************************
    // Return the results as separate matrices
    plhs[0] = mxCreateDoubleMatrix(numOutputDimensions, reporterEF->getNumStepsSimulated(), mxREAL);
    double* ptrOut = (double*)mxGetPr(plhs[0]);
    reporterEF->fillEFPositionsMatrix(ptrOut);

    //******************************
    // Save Mot File if runId < 1
    double* fileNameId_ptr = mxGetPr(prhs[3]);
    int suffixId = (int)fileNameId_ptr[0];
    if(suffixId > 0)
    {
        char buff[50];
        sprintf(buff, "Learned_Skill_ArmOrig_Id%d.mot",suffixId);

        Storage statesDegrees(manager.getStateStorage());
        osimModel.updSimbodyEngine().convertRadiansToDegrees(statesDegrees);

        statesDegrees.setWriteSIMMHeader(true);
        statesDegrees.print(buff);
        
        osimModel.printControlStorage("Arm26Orig_controls.sto");
        manager.getStateStorage().print( "Arm26Orig_states.sto" );
    }
        
    
}
