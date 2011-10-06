#ifndef MATRIXTASK_H
#define MATRIXTASK_H

#include <limits.h>
#include <string>
#include <vector>
#include <assert.h>

namespace DRT{
	class AbstractDRTBuilder;
namespace Matrix{
	class MatrixDRTBuilder;

/** Matrix representation of a task */
class MatrixTask{
	/** Internal representation of a job */
	struct Job{
		std::string name;
		int wcet;
		int deadline;
	};

	MatrixTask(size_t jobs){
		_mtimes = NULL;
		_jobs = NULL;
		_nb_jobs = jobs;
	}
public:
	/** Type definition of job id */
	typedef unsigned int JobId;

	~MatrixTask(){
		if(_mtimes){
			delete[] _mtimes;
			_mtimes = NULL;
		}
		if(_jobs){
			delete[] _jobs;
			_jobs = NULL;
		}
	}

	/** Name of this task */
	const std::string& name() const { return _name; }

	/** Number of jobs */
	size_t jobs() const { return _nb_jobs; }

	/** Get jobid of a job */
	JobId job(const std::string& name) const{
		for(int i = 0; i < _nb_jobs; i++){
			if(_jobs[i].name == name)
				return i;
		}
		assert(false);
		return -1;
	}

	/** Name of a job */
	const std::string& name(JobId job) const { return _jobs[job].name; }

	/** Worst case execution time of a job */
	int wcet(JobId job) const { return _jobs[job].wcet; }

	/** Deadline of a job */
	int deadline(JobId job) const { return _jobs[job].deadline; }

	/** Minimum inter-release time between two jobs, INT_MAX if no inter-release time */
	int mtime(JobId src, JobId dst) const {
		return _mtimes[src + jobs()*dst];
	}

	/** True, if there's is an edge between src and dst */
	bool edge(JobId src, JobId dst) const{
		return _mtimes[src + jobs()*dst] != INT_MAX;
	}

	void produce(AbstractDRTBuilder* builder) const;
	static void produce(const std::vector<MatrixTask*>& tasks, AbstractDRTBuilder* builder);
private:
	std::string	_name;			///< Name of the task
	Job* _jobs;					///< Jobs in the task
	size_t _nb_jobs;			///< Number of jobs
	int* _mtimes;				///< Minimum inter-release time between two jobs, negative if no edge

	friend class MatrixDRTBuilder;
};

} /* Matrix */
} /* DRT */

#endif /* MATRIXTASK_H */
