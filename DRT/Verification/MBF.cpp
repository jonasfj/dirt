#include "MBF.h"

using namespace DRT::Matrix;

namespace DRT{
namespace Verification{

MBF::MBF(const MatrixTask* task, size_t* mod_weight){
	_task = task;
	_mod_weight = mod_weight;
}

std::vector<Matrix::MatrixTask::JobId> MBF::run(Matrix::MatrixTask::JobId init){
	//int distance[entries];

	/* initiating distance matrix, setting all distances to 'infinite' */
	for(Matrix::MatrixTask::JobId j = 0; j < _task->jobs()*_task->jobs(); j++){
		distance[j] = INT_MAX;//n
	}
	distance[init]  = 0;

	/* initiating predecessor list */
	for(Matrix::MatrixTask::JobId j = 0; j < _task->jobs(); j++){
		pred[j] = INT_MAX;//n
	}
    
    std::vector<Matrix::MatrixTask::JobId> passed;	//To avoid following the same path twice.
	std::vector<Matrix::MatrixTask::JobId> cycle;	//We save the nodes that form a potential cycle.
	
	while(update_dist_and_pred()){
		passed.clear();

		for(Matrix::MatrixTask::JobId j = 0; j < sizeof(pred); j++){
			if(pred[j] != INT_MAX){
				cycle.clear();
				Matrix::MatrixTask::JobId q = pred[j];
				while(q != INT_MAX){// && cycle[q] != -1 && passed[q] != -1){
					
					/* find out if q is in cycle or passed so far. */
					size_t i = 0;
					while(cycle[i] != q && i < cycle.size())
						i++;
					if(i != cycle.size())
						continue;
					i = 0;
					while(passed[i] != q && i < passed.size())
						i++;
					if(i != passed.size())
						continue;
					
					/* at q to the cycle list */
					cycle.push_back(q);
					
					q = pred[q];
					
				}
				/* find out if q is in cycle or passed so far. If not, it is not a cycle and we add to the passed list. */
				size_t i = 0;
				while(cycle[i] != q && i < cycle.size())
					i++;

				/* Was q found? Then remove all previous nodes in the list that are not part of the cycle. We have found a cycle. */
				if(i != cycle.size()){
					while(1 != cycle.at(0))
						cycle.erase(cycle.begin());
					return cycle;
				}
				/* If not, add all nodes that we have gone through to the passed list and update the graph again. */
				else{
					while(cycle.size() != 0){
						passed.push_back(cycle.at(0));
						cycle.erase(cycle.begin());
					}
				}
			}
		}
	}
	cycle.clear();
	return cycle;
}

bool MBF::update_dist_and_pred(){
	bool changes = false;
	for(Matrix::MatrixTask::JobId i = 0; i < _task->jobs(); i++){//n
		for(Matrix::MatrixTask::JobId j = 0; j < _task->jobs(); i++){//n - n²
			if(_task->edge(i,j)){
				size_t new_dist = distance[i] + _mod_weight[i + _task->jobs()*j];
				if (new_dist < distance[j]){
					distance[j] = new_dist;
				}
				pred[j] = i;
				changes = true;
			}
		}
	}
    return changes;
}

} /* Verification */
} /* DRT */
