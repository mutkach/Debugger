#include <stdio.h>

#include "BPatch.h"
#include "BPatch_point.h"
//#include "BPatch_process.h"
#include "BPatch_function.h"
//#include "BPatch_thread.h"
//#include <vector>
//#include "BPatch_addressSpace.h"
//#include "BPatch_binaryEdit.h"
#include "BPatch_flowGraph.h"

BPatch bpatch;

std::vector<BPatch_function *> findEntryPoint(BPatch_addressSpace *app) {
    std::vector<BPatch_function *> functions;
    std::vector<BPatch_point *> *points;
    BPatch_image *appImage = app->getImage();
    appImage->getProcedures(functions);
    //appImage->findFunction("InterestingProcedure", functions);
    //points = functions[0]->findPoint(BPatch_entry);

    return functions;
}

BPatch_addressSpace *startInstrumenting(const char *name,
                                       // int pid, // For attach
                                        const char *argv[]) { // For create
    BPatch_addressSpace *handle = NULL;
    handle = bpatch.processCreate(name, argv);

}
int main() {
    const char *progName = "/usr/games/pacman";
    const char *progArgv[] = {"/usr/games/pacman", NULL};
    BPatch_addressSpace *app = startInstrumenting(progName, progArgv);
 //Example 2: get entry point
    std::vector<BPatch_function *> functions = findEntryPoint(app);
    for(int i = 0; i < functions.size(); ++i){
        printf("%s\n", functions[i]->getDemangledName().c_str());

    }



    return 0;
}