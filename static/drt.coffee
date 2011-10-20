# Ensure that object this.drt exists, as we'll add to this namespace
@drt ?= {}

log = (msg) -> if console? and console.log? then console.log(msg) else alert(msg)

# Representation of a task
class @drt.Task
	constructor: (@name) ->
	_delay: []
	jobs: []
	delay: (source, target) =>
		# Identify if objects
		source = source.id if typeof source is "object"
		target = target.id if typeof target is "object"
		return @_delay[source + @jobs.length * target]
	edge:  (source, target) =>
		return @delay(source, target) != -1
	addJob: (job) =>
		o_d = @_delay
		o_l = @jobs.length
		job.id = @jobs.length
		@jobs[job.id] = job
		@_delay = []
		for i in [0..@jobs.length-1]
			for j in [0..@jobs.length-1]
				@_delay[i + @jobs.length * j] = -1
		for i in [0..o_l-1]
			for j in [0..o_l-1]
				@_delay[i + @jobs.length * j] = o_d[i + o_l * j]
		return
	addEdge: (source, target, delay) =>
		# Identify if objects
		source = source.id if typeof source is "object"
		target = target.id if typeof target is "object"
		# Set the delay
		@_delay[source + @jobs.length * target] = delay
		# Maintain preset and postset
		@jobs[source].postset.push(@jobs[target]) unless @jobs[target] in @jobs[source].postset
		@jobs[target].preset.push(@jobs[source]) unless @jobs[source] in @jobs[target].preset
		return
	removeEdge: (source, target) =>
		@addEdge(source, target, -1)
		return
	job: (id) => return @jobs[id]
	id: (name) =>
		for job in @jobs
			return job.id if job.name == name
		return -1

# Representation of a job
class @drt.Job
	@Types = {Job: {}, Fork: {}, Merge: {}}
	constructor: (@name, @wcet, @deadline, @type = Job.Types.Job, @id = -1) ->
		@preset = []
		@postset = []

# Parse Xml using jquery
@drt.parseXml = (xml) ->
	tasks = []
	xml = jQuery.parseXML(xml)
	$(xml).find("drt").each ->
		$(this).find("task").each ->
			task = new drt.Task($(this).attr("name"))
			$(this).find("job").each ->
				id = task.jobs.length
				task.jobs[id] = new drt.Job($(this).attr("name"),
											 $(this).attr("wcet"),
											 $(this).attr("deadline"),
											 drt.Job.Types.Job, id)	#TODO: Different job types
			for i in [0..task.jobs.length-1]
				task._delay[i + task.jobs.length * j] = -1 for j in [0..task.jobs.length-1]
			$(this).find("edge").each ->
				task.addEdge(task.id($(this).attr("source")),
							 task.id($(this).attr("destination")),
							 $(this).attr("delay"))
			tasks.push(task)
	return tasks

# Validate Well-formedness of task and parallel isolation property
@drt.validate = (task, output = log) ->
	retval = true
	assert = (cond, msg) =>
		unless cond
			output "validation-error: #{msg}"
			retval = false
	# Check that names are unique
	names = []
	for job in task.jobs
		if job.name in names
			assert false, "Job name: \"#{job.name}\" is not unique in task: #{task.name}"
		else
			names.push(job.name)
	# TODO Validate job ids and correct ordering of job ids
	# Validate preset/postset
	for source in task.jobs
		for target in task.jobs
			if task.edge(source, target)
				assert source in target.preset, "#{source.name} not in preset of #{target.name}"
				assert target in source.postset, "#{target.name} not in postset of #{source.name}"
			if source in target.preset
				assert task.edge(source, target), "#{source.name} is in preset of #{target.name}"
			if target in source.postset
				assert task.edge(source, target), "#{target.name} is in postset of #{source.name}"
	# Verify frame separation property
	for source in task.jobs
		for target in source.postset
			assert task.delay(source, target) >= source.deadline,
				"frame separation property violation from #{source.name} to #{target.name}"
	# Compute parents
	P = []
	P[j.id] = new Set() for j in task.jobs			#Initially empty
	for f in task.jobs when f.type is Job.Types.Fork
		P[j.id].add([f.id, j.id]) for j in f.postset	#Trivial rule
	for i in [0..task.jobs.length]
		# Jobs propogate to jobs and forks
		for v in task.jobs when j.type is Job.Types.Job and P[j.id].length() > 0
			for u in v.postset when y.type isnt Job.Types.Merge and not P[u.id].subset(P[j.id])
				P[u] = P[u].union(P[j.id])
		# Merges looks ingoing jobs and forks, adopts their grand parents
		for m in task.jobs when m.type is Job.Types.Merge
			forks_handled = []
			for v in m.preset
				for [f, u] in P[v.id].elements when not f in forks_handled
					P[m.id] = P[m.id].union(P[f.id])
					forks_handled.push(f)
	# Verify the parallel isolation property
	for j in task.jobs
		assert P[j.id].length() <= 1, "#{j.name} is not parallel isolated, it is reachable
									   from #{f.name for [f, v] in P[j.id].elements}"
	return retval

# Set of tuples
class Set
	constructor: (@elements = []) ->
	subset: (set) =>
		return false if set.elements.length > @elements.length
		for e in @elements
			return false unless set.contains(e)
		return true
	union: (set) =>
		u = []
		for e in @elements
			u.push(e)
		for e in set.elements
			u.push(e) unless @contains(e)
		return Set(u)
	contains: (element) =>
		for e in @elements when e.length == element.length
			retval = true
			for i in [0..e.length]
				if e[i] isnt element[i]
					retval = false
					break
			return true if retval
		return false
	add: (element) =>
		@elements.push(Object.freeze(element)) unless @contains(element)
	length: => @elements.length


# Discrete Demand Bound Function
class DiscreteDBF
	constructor: (@dominates, @tuples = []) =>
		@resort()
	resort: =>
		tuples = @tuples
		@tuples = []
		insert(tuple) for tuple in tuples
	insert: (tuple) =>
		i = @search(tuple.time)			# @tuples[i].time >= tuple.time
		# Check if tuple is dominated
		return false if i > 0 and @dominates(@tuples[i-1], tuple)
		return false if i != @tuples.length and @dominates(@tuples[i], tuple)
		d = i
		d = d + 1 while d != @tuples.length and @dominates(tuple, @tuples[d])
		low = @tuples[0...i]
		low.push(tuple, @tuples[d...@tuples.length]...)
		@tuples = low
		return true
	search: (time) => 
		# Find index of first element >= time
		# TODO Use binary search, see C++ sources
		for i in [0..@tuples.length] when @tuples[i].time >= time
			return i
		return @tuples.length
	pop: =>
		# Remove tuple from list
		return @tuples.pop()

# Compute utilization of a task
@drt.utilization = (task) ->
	# Checks for domination
	dominates = (t1, t2) -> return t1.time =< t2.time and t1.wcet >= t2.wcet
	# Create tuple list and waiting list for each job
	wLists = []; tList = []
	for j in task.jobs
		wLists.push(new DiscreteDBF(dominates))
		tLists.push(new DiscreteDBF(dominates))
	# Create initial tuples
	for job in task.jobs
		t = wcet: 0, time: 0, start: job, parent: null
		wLists[job.id].insert(t)
		tLists[job.id].insert(t)
	# Define how we expand tuples
	U = 0
	expandTuples = (job) ->
		gotNews = false
		while t = wLists[job.id].pop()
			for j in task.jobs when task.edge(job, j)
				n =
					wcet: t.wcet + j.wcet
					time: t.time + task.delay(job, j)
					start: t.start
					parent: t.parent
				if n.start is j	# Never insert loops
					if n.wcet / n.time > U
						U = n.wcet / n.time
				else if tLists[j.id].insert(n)
					gotNews |= wLists[j.id].insert(n)
		return gotNews
	# Expand tuples till we're done
	for i in [0...task.jobs.length]
		gotNews = false
		for job in task.jobs
			gotNews |= expandTuples(job)
		break unless gotNews
	return U



