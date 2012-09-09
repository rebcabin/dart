#include <iostream>
#include <string>
using namespace std;

#include "kinematics/Skeleton.h"
#include "kinematics/FileInfoC3D.h"
#include "kinematics/FileInfoDof.h"
using namespace kinematics;
#include "IKProblem.h"
#include "PositionConstraint.h"
#include "optimizer/ObjectiveBox.h"
using namespace optimizer;

#include "optimizer/snopt/SnoptSolver.h"

using namespace Eigen;

#include "utils/Paths.h"

DEFINE_string(skel, DART_DATA_PATH"skel/Nick01.vsk",
              "The input Skel file");
DEFINE_string(c3d, DART_DATA_PATH"c3d/nick_freeform_001.c3d",
              "The input C3D file");
DEFINE_string(dof, DART_DATA_PATH"dof/result.dof",
              "The output dof file");


int main(int argc, char* argv[]) {
    // Init google libraries
    ParseCommandLineFlags(&argc, &argv, true);

    // Define logging flag
    cout << "todo: take argv options for log level\n" << endl;

    string filename_skel = FLAGS_skel;
    string filename_c3d = FLAGS_c3d;
    string filename_dof = FLAGS_dof;

    cout << "[INFO]" << "filename_skel = " << filename_skel;
    cout << "[INFO]" << "filename_c3d  = " << filename_c3d;
    cout << "[INFO]" << "filename_dof  = " << filename_dof;

    cout << "[INFO]" << "simpleik begins";

    IKProblem prob(filename_skel.c_str());

    FileInfoC3D c3dFile;
    bool result = c3dFile.loadFile(filename_c3d.c_str());
    CHECK(result);

    FileInfoDof resultDof(prob.getSkel());

    snopt::SnoptSolver solver(&prob);
    for (int i = 0; i < c3dFile.getNumFrames(); i++) {
        cout << "[INFO]" << "Frame Index = " << i;
        for (int j = 0; j < c3dFile.getNumMarkers(); j++) {
            // PositionConstraint* p = dynamic_cast<PositionConstraint*>(prob.objBox()->getConstraint(j));

            PositionConstraint* p = prob.getConstraint(j);

            Vector3d target = c3dFile.getDataAt(i, j);
            p->setTarget(target);

            bool isInBox = (prob.objBox()->isInBox(p) != -1);
            bool isMissing = (target.norm() < 0.001);

            if (isMissing && isInBox) {
                int result = prob.objBox()->remove(p); 
                cout << "[INFO]" << "\tDetect missing marker " << j << " ";
                cout << "[INFO]" << "TakeOut() = " << result << " "
                          << "size = " << prob.objBox()->getNumConstraints();

            }
            else if (!isMissing && !isInBox) {
                prob.objBox()->add(p);
                cout << "[INFO]" << "\tWe got the marker " << j << " back!!! ";
                cout << "[INFO]" << "size = " << prob.objBox()->getNumConstraints();
            }

        }
        cout << "[INFO]" << "Update OK";

        solver.solve();

<<<<<<< HEAD
        vector<double> pose;
=======
        VectorXd pose;
>>>>>>> karen
        prob.getSkel()->getPose(pose);
        resultDof.addDof(pose);
    }
    
    resultDof.saveFile(filename_dof.c_str(), 0, resultDof.getNumFrames());
    cout << "[INFO]" << "Save the result to [" << filename_dof.c_str() << "]";
    cout << "[INFO]" << "Save OK";

    cout << "[INFO]" << "simpleik OK";
}
