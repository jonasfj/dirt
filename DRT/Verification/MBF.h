#ifndef MBF_H
#define MBF_H

#include "../Matrix/MatrixTask.h"

#include <vector>

namespace DRT{
	namespace Verification{

		/** Algorithm for finding a negative cycle in the task graph */
		class MBF{
			public:
				MBF(const Matrix::MatrixTask* task, size_t* mod_weight);
				std::vector<Matrix::MatrixTask::JobId> run(Matrix::MatrixTask::JobId init);
			private:
				const Matrix::MatrixTask* _task;						///< MatrixTask being processed
				size_t* _mod_weight;
				bool* visited;											///< Nodes which have been visited in the deep search process. Assumes jobs are enumerated continously
				size_t* distance;										///< Distance between nodes in the graph
				Matrix::MatrixTask::JobId* pred;											///< Predecessor list telling which predecessor node is the cheapest
				std::vector<Matrix::MatrixTask::JobId> predecessorList;
				std::vector<std::vector<Matrix::MatrixTask::JobId> > cycleList;
				bool update_dist_and_pred();
		};

	} /* Verification */
} /* DRT */

#endif /* MBF_H */

