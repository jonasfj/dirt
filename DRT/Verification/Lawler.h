#ifndef LAWLER_H
#define LAWLER_H

#include "../Matrix/MatrixTask.h"
#include "MBF.h"
#include <vector>

namespace DRT{
	namespace Verification{

		/** Algorithm for finding a negative cycle in the task graph */
		class Lawler{
			public:
				static double computeUtilization(const std::vector<Matrix::MatrixTask*>& tasks);
				double computeUtilization(const Matrix::MatrixTask* task);
			private:
				const Matrix::MatrixTask* _task;						///< MatrixTask being processed
				size_t* _mod_weight;
				void Update_weights();
				size_t lambda;
		};

	} /* Verification */
} /* DRT */

#endif /* LAWLER_H */


