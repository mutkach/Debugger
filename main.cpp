#include <stdio.h>

#include "BPatch.h"
#include "BPatch_point.h"
#include "BPatch_function.h"
#include "BPatch_flowGraph.h"
#include <queue>

BPatch bpatch;
std::vector<BPatch_function *> printfFuncs;
std::set<std::string> visited;
BPatch_image *appImage;
BPatch_addressSpace *app;
const int length = 60;

BPatch_addressSpace *startInstrumenting(const char *name,
                                        const char **argv) { // For create
    BPatch_addressSpace *handle = NULL;
    handle = bpatch.processCreate(name, argv);
    return handle;

}

void createAndInsertStamp(std::string funcName, std::vector<BPatch_point *> &point) {
    std::vector<BPatch_snippet *> printfArgs;
    std::string inject = funcName;
    inject += "\n";
    BPatch_snippet *fmt = new BPatch_constExpr(inject.c_str());
    printfArgs.push_back(fmt);
    BPatch_funcCallExpr printfCall(*(printfFuncs[0]), printfArgs);
    app->insertSnippet(printfCall, point);

}


void cfgTraversal(BPatch_flowGraph* start, std::queue<BPatch_flowGraph*> *order){
    std::set<BPatch_basicBlock *>::iterator block_iter;
    std::set<BPatch_basicBlock *> blocks;
    start->getAllBasicBlocks(blocks);
    start->createSourceBlocks();

    for (block_iter = blocks.begin(); block_iter != blocks.end(); ++block_iter)
    {
        BPatch_basicBlock *block = *block_iter;
        BPatch_Vector<BPatch_sourceBlock*> source_block;

        block->getSourceBlocks(source_block);
        if(!source_block.empty()){
            printf("non empty source in - %s\n", start->getFunction()->getDemangledName().c_str());
        }
        BPatch_function* call = block->getCallTarget();
        //std::vector<unsigned short> lines;
        //source_block[0]->getSourceLines(lines);
        if(call != NULL && visited.find(call->getDemangledName())==visited.end()) {
            std::vector<BPatch_point* >points;
            std::string funcName = call->getDemangledName();
            std::string from = start->getFunction()->getDemangledName();
            std::string stamp = from;
            points.push_back(block->findEntryPoint());
            stamp = from;
            //stamp += to_string(*lines.begin()) + " " +to_string(*lines.end());
            for(int i = 0; i < length - from.size(); ++i){
                if(i + from.size() == length/2){
                    stamp += "-->";
                    i+=2;
                }
                else {
                    stamp += " ";
                }
            }
            stamp += funcName;
            createAndInsertStamp(stamp, points);
            visited.insert(funcName);
            order->push(call->getCFG());
        }
    }
}


void BreadthTraverse(BPatch_flowGraph* start){
    std::queue <BPatch_flowGraph*> order;
    order.push(start);
    while(!order.empty()){
        BPatch_flowGraph* current = order.front();
        cfgTraversal(current, &order);
        order.pop();
    }
}




int main(int argc, const char** argv) {
    const char *progName = argv[1];
    const char **progArgv = &argv[1];
    bpatch.setTrampRecursive(false);
    bpatch.setMergeTramp(false);
    bpatch.setSaveFPR(false);
    bpatch.setDelayedParsing(true);
    bpatch.setTypeChecking(false);
    bpatch.setInstrStackFrames(false);
    bpatch.setDebugParsing(true);
    bpatch.parseDebugInfo();
    time_t speed, speed_ok;
    time(&speed);
    app = startInstrumenting(progName, progArgv);
    std::vector<BPatch_module *> modules;

    appImage = app->getImage();
    appImage->getModules(modules);


    time(&speed_ok);
    printf("image got in -- %f\n", difftime(speed_ok,speed));

    std::vector<BPatch_function *> mainFunction;
    appImage->findFunction("main", mainFunction);
    time(&speed_ok);
    printf("main found in -- %f\n", difftime(speed_ok,speed));
    appImage->findFunction("printf", printfFuncs);
    time(&speed_ok);
    printf("printf found in -- %f\n", difftime(speed_ok,speed));

    //BPatch_flowGraph* cfg = mainFunction[0]->getCFG();
    std::vector<BPatch_function*> default_functions;
    modules[0]->getProcedures(default_functions);
    for(int i = 0; i < default_functions.size(); ++i){
        printf("%s\n", default_functions[i]->getDemangledName().c_str());
        BreadthTraverse(default_functions[i]->getCFG());
    }
    //std::set<BPatch_basicBlock *> blocks;
    //cfg->getAllBasicBlocks(blocks);


    //printf("blocks size: %d\n\n\n", blocks.size());
//    recursiveTraversal(cfg);




    time(&speed_ok);
    printf("injection finished, time -- %f\n", difftime(speed_ok,speed));
    //fflush(stdout);


    BPatch_process *appProc = dynamic_cast<BPatch_process *>(app);
    appProc->detach(true);

    appProc->continueExecution();
    return 0;
    /*
    while (!appProc->isTerminated())
        bpatch.waitForStatusChange();

    return 0;
     */
}