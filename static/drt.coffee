# Ensure that object this.drt exists, as we'll add to this namespace
@drt ?= {}

log = (msg) -> if console? and console.log? then console.log(msg) else alert(msg)

# Representation of a task
class @drt.Task
	constructor: (@name, jobs = []) ->
		@_delay = []
		@jobs = []
		for job, i in jobs
			@jobs[i] = job
			job.id = i
		for i in [0...@jobs.length]
			for j in [0...@jobs.length]
				@_delay[i + @jobs.length * j] = -1
	delay: (source, target) =>
		# Identify if objects
		source = source.id if typeof source is "object"
		target = target.id if typeof target is "object"
		return @_delay[source + @jobs.length * target]
	edge:  (source, target) =>
		return @delay(source, target) != -1
	addEdge: (source, target, delay) =>
		# Identify if objects
		source = source.id if typeof source is "object"
		target = target.id if typeof target is "object"
		# Set the delay
		@_delay[source + @jobs.length * target] = parseInt(delay)
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
	@Types = {Job: "job", Fork: "fork", Merge: "merge"}
	constructor: (@name, @wcet, @deadline, @type = Job.Types.Job, @x = 0, @y = 0, @id = -1) ->
		@preset = []
		@postset = []
		@parent = null

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
											 $(this).attr("type") ? drt.Job.Types.Job,
											 $(this).attr("x") ? 0,
											 $(this).attr("y") ? 0,
											id)
			for i in [0..task.jobs.length-1]
				task._delay[i + task.jobs.length * j] = -1 for j in [0..task.jobs.length-1]
			$(this).find("edge").each ->
				task.addEdge(task.id($(this).attr("source")),
							 task.id($(this).attr("target")),
							 $(this).attr("delay"))
			tasks.push(task)
	return tasks

# Generate Xml from task
@drt.writeXml = (tasks...) ->
	xml = "<drt>\n"
	for task in tasks
		xml += "\t<task name=\"#{task.name}\">\n"
		for job in task.jobs
			xml += "\t\t<job "
			xml += "name=\"#{job.name}\" "
			xml += "wcet=\"#{job.wcet}\" "
			xml += "deadline=\"#{job.deadline}\" "
			xml += "type=\"#{job.type}\" "
			xml += "x=\"#{job.x}\" y=\"#{job.y}\"/>\n"
		for src in task.jobs
			for dst in task.jobs when task.edge(src, dst)
				xml += "\t\t<edge source=\"#{src.name}\" target=\"#{dst.name}\" delay=\"#{task.delay(src, dst)}\"/>\n"
		xml += "\t</task>\n"
	return xml + "</drt>\n"

# Create a simple tuple
Tuple = (tuple...) -> return Object.freeze(tuple)

# Compute parents returns true if successful
@drt.computeParents = (task) ->
	# Reset all parents
	bad = false
	j.parent = null for j in task.jobs
	for f in task.jobs when f.type is drt.Job.Types.Fork
		j.parent = Tuple(f, j.id) for j in f.postset
	for i in [0..task.jobs.length]
		# Jobs propogate to jobs and forks
		for j in task.jobs when j.type is drt.Job.Types.Job and j.parent?
			for u in j.postset when u.type isnt drt.Job.Types.Merge and u.parent isnt j.parent
				bad = true if u.parent?
				u.parent = j.parent
		# Merges looks ingoing jobs and forks, adopts their grand parents
		for m in task.jobs when m.type is drt.Job.Types.Merge
			for j in m.preset when j.parent? and j.parent[0].parent? and j.parent[0].parent isnt m.parent
				bad = true if m.parent?
				m.parent = j.parent[0].parent
	unless bad
		return true
	else
		j.parent = null for j in task.jobs	# Reset parents
		return false

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
	# Validate job ids and correct ordering of job ids
	for i in [0...task.jobs.length]
		assert task.jobs[i].id is i, "Job \"#{task.jobs[i].name}\" has out-of-order id"
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
				"Frame separation property violation from #{source.name} to #{target.name}"
	# Compute parents
	assert drt.computeParents(task), "Task isn't parallel isolated"
	# Check trace auxiliary function
	check_trace = (parent, fork, visited = [])->
		if not parent?
			return true
		if parent[0] == fork or parent in visited
			return false
		visited.push(parent)
		return check_trace(parent[0].parent, fork, visited)
	# Check trace for all forks
	for f in task.jobs when f.type is drt.Job.Types.Fork
		assert check_trace(f.parent, f), "Fork #{f.name} can fork itself infinitely"
	# Check cardinality for all forks and merges
	for f in task.jobs when f.type is drt.Job.Types.Fork
		assert f.postset.length == 2, "postset of #{f.name} isn't 2"
	for m in task.jobs when m.type is drt.Job.Types.Merge
		assert m.preset.length == 2, "preset of #{m.name} isn't 2"
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
	constructor: (@dominates, @tuples = []) ->
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
	dominates = (t1, t2) -> return t1.time <= t2.time and t1.wcet >= t2.wcet
	# Create waiting list, tuple list and cycle list for each job
	wLists = []; tLists = []; cLists
	for j,i in task.jobs
		wLists.push(new DiscreteDBF(dominates))
		tLists.push(new DiscreteDBF(dominates))
		cLists.push(new DiscreteDBF(dominates))
	# Create initial tuples
	for job in task.jobs
		t = wcet: 0, time: 0, start: job, parent: null, visited: new Bits(task.jobs.length, 0)
		wLists[job.id].insert(t)
		tLists[job.id].insert(t)
	# Define how we expand tuples
	expandTuples = (job) ->
		gotNews = false
		while t = wLists[job.id].pop()
			# Use t as parent if job is a fork
			p = if job.type is drt.Job.Types.Fork then t else t.parent
			for j in task.jobs when task.edge(job, j) and not t.visited.test(j.id)
				v = t.visited.clone()
				v.set(j.id)
				n =
					wcet: t.wcet + j.wcet
					time: t.time + task.delay(job, j)
					start: t.start
					parent: p
					visited: v
				# TODO Handle special case when j is a merge, we need to attempt merge with tuples at
				# preset of j
				if n.start is j
					# Insert cycles in cycle list, and never in waiting list!
					cLists[j.id].insert(n)
				else if tLists[j.id].insert(n)
					gotNews |= wLists[j.id].insert(n)
			# Forget visited of t as it'll only exist in tLists
			delete t.visited
		return gotNews
	# Expand tuples till we're done
	for i in [0...task.jobs.length]
		gotNews = false
		for job in task.jobs
			gotNews |= expandTuples(job)
		break unless gotNews
	# Define how we merge cycles
	mergeCycles = (job) ->
		gotNews = false
		for j in task.jobs when canMergeByParent(j.parent, job.parent)
			for c1 in cLists[job.id]
				for c2 in cLists[j.id].tuples when c1.parent == c2.parent
					res = merge(c1, c2)
					if j.parent?
						id = j.parent[0].id
					else
						id = j.id	#Not sure if this is smart...
					gotNews |= cLists[id].insert(res)	#Not sure if domination is okay here...
		return gotNews
	# Find utilization...
	
	return U


