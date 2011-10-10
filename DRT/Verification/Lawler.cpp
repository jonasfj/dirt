#include "Lawler.h"

using namespace DRT::Matrix;

namespace DRT{
namespace Verification{

double Lawler::computeUtilization(const std::vector<Matrix::MatrixTask*>& tasks){
	double U = 0;
	for(std::vector<MatrixTask*>::const_iterator it = tasks.begin(); it != tasks.end(); it++)
		U += Lawler().computeUtilization(*it);
	return U;
}

        /* Linear Search variant of Ratio Optimization algorithm by Lawler. Also known as Dinkelbachs method */
double Lawler::computeUtilization(const MatrixTask* task){
	_task = task;
	lambda = 0;
    
    for(Matrix::MatrixTask::JobId i = 0; i < _task->jobs(); i++){//n
		lambda += _task->wcet(i);
	}

	bool negative = true;
	std::vector<Matrix::MatrixTask::JobId> cycle;
	size_t execSum = 0;
	size_t timeSum = 0;
	
	while(negative){
		Update_weights();
		size_t weightSum = 0;

		DRT::Verification::MBF mbf(_task,_mod_weight);
		//cycle = mbf.run(0);
		if(cycle.size() == 0)
			break;
		else{
			for(Matrix::MatrixTask::JobId j = 0; j < cycle.size()-1; j++){
				weightSum += _mod_weight[cycle.at(j) + _task->jobs()*cycle.at(j+1)];
			}
		}

		if(weightSum >= 0){
			for(Matrix::MatrixTask::JobId j = 0; j < cycle.size()-1; j++){
				execSum += _task->wcet(cycle.at(j));
				timeSum += _task->mtime(cycle.at(j),cycle.at(j+1));
			}
			execSum += _task->wcet(cycle.at(cycle.size()-1));
			negative = false;
		}
	}

	return execSum/timeSum;
}

void Lawler::Update_weights(){
	for(Matrix::MatrixTask::JobId i = 0; i < _task->jobs()-1; i++){//n
		for(Matrix::MatrixTask::JobId j = 0; j < _task->jobs()-1; j++){//n - n²
			if(_task->edge(i,j)){ //D: giver segmentfejl
				_mod_weight[i+ _task->jobs()*j] = _task->wcet(i) - lambda*_task->mtime(i,j); //D: giver segmentfejl
			}
		}
	}
}

} /* Verification */
} /* DRT */

