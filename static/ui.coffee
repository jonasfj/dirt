@dirt ?= {}

@dirt.main = =>
	# $("#add-job").button()
	$("#jobs").buttonset()
	$("#validate").button()
	$("#open").button()
	$("#save").button()
	$("#mode").buttonset()

	$(".widget").addClass("no-select")

	# Initial jsPlumb canvas
	jsPlumb.ready(dirt.initJsPlumb)

	# Make sure we clean up after jsPlumb
	$(window).unload -> jsPlumb.unload()

	# Open file when file dialog file is changed
	#$("#file-dialog-file").change(@dirt.loadInput)
	document.getElementById("file-dialog-file").addEventListener("change", @dirt.readFile, false)
	
	$("#mode-model").click ->
		$("#add-job, #add-merge, #add-fork, #open, #validate, #save").button("enable")
		$("#analysis").fadeOut "fast", ->
			$("#document").fadeIn "fast", ->
				$("#analysis").empty()
	$("#mode-verify").click ->
		$("#add-job, #add-merge, #add-fork, #open, #validate, #save").button("disable")
		$("#document").fadeOut "fast", ->
			$("#analysis").fadeIn "fast", dirt.analyze
	# Dialog stuff open file
	$("#open").click ->
		unless window.File and window.FileReader and window.FileList
			alert("Browser support lacking")
			return
		$("#file-dialog").dialog
								modal: true
								resizable: false
								closeText: "cancel"
								draggable: false
	$("#save").click ->
		#TODO Code to detect if the browser supports this, IE obviously have some limits
		xml = "data:application/drt+xml;base64,#{$.base64Encode(drt.writeXml(dirt.getTask()))}"
		window.open(xml, "_blank")

# Init JsPlumb stuff
@dirt.initJsPlumb = ->
	jsPlumb.setRenderMode(jsPlumb.SVG)
	# Set container for the document
	jsPlumb.Defaults.Container = "document"
	jsPlumb.Defaults.DragOptions = cursor: "pointer", zIndex: 2000
	jsPlumb.Defaults.EndpointStyle = radius: 6, fillStyle: "#fff"
	jsPlumb.Defaults.Endpoint = "Dot"
	jsPlumb.Defaults.EndpointHoverStyle =  strokeStyle: "#333", fillStyle: "#ccc" 
	jsPlumb.Defaults.MaxConnections = -1
	jsPlumb.Defaults.PaintStyle = strokeStyle: "gray", lineWidth: 2
	jsPlumb.Defaults.Overlays = [
		["Arrow", { width:10,length:6, location: 0.25}],
		["Arrow", { width:10,length:6, location: 0.75}],
		["Label", { location: 0.5, label: (ci) -> return if ci.connection.delay? then "#{ci.connection.delay}" else "", 
		cssClass:"label no-select"}]
	]
	
	jsPlumb.bind "jsPlumbConnection", (connInfo) -> 
		src = $("#" + connInfo.sourceId).data("endpoint")
		end = $("#" + connInfo.targetId).data("endpoint")
		connInfo.connection.delay = 1
		count = 0
		for conn in src.connections
			if (conn.targetId == connInfo.targetId and 
				conn.sourceId == connInfo.sourceId)
					count = count + 1
		if (count > 1)
			src.detach(connInfo.connection)

	# Click handler for adding jobs, forks and merges
	$("#add-job").click -> dirt.adding = drt.Job.Types.Job
	$("#add-fork").click -> dirt.adding = drt.Job.Types.Fork
	$("#add-merge").click -> dirt.adding = drt.Job.Types.Merge
	# Event handler for validation
	$("#validate").click ->
		task = dirt.getTask()
		if dirt.validate(task)
			$.gritter.add
						title: "Task validation successfull"
						text: "Task \"#{task.name}\" was validated and satisfies the parallel isolation property."
	# Click handler for insering stuff...
	$("#document").click (args) ->
		if dirt.adding
			dirt.addJob(dirt.getNextJobName(), 0, 1, dirt.adding, args.pageX, args.pageY) 
			dirt.adding = null

	# Set event handler for editing edge
	jsPlumb.bind "dblclick", dirt.editEdge

# Helper function to dump all properties of an object
inspect = (d) -> console.log("#{d}\n" + ("#{i}: #{k}, " for i, k of d).join())

# which kind of job is being added
@dirt.adding = null

@dirt.getNextJobName = ->
	number = 0
	number = number + 1 while document.getElementById(dirt.name2id("j_#{number}"))
	return "j_#{number}"

# Edits a job
@dirt.editJob = (jobId) ->
	$("#job-dialog-name").val($("#" + jobId).data("name"))
	$("#job-dialog-wcet").val($("##{jobId}").data("wcet"))
	$("#job-dialog-deadline").val($("##{jobId}").data("deadline"))
	$("#job-dialog").dialog
		modal: true
		resizable: false
		closeText: "cancel"
		draggable: false
		buttons:
			"Apply": ->
				name = $("#job-dialog-name").val()
				wcet = $("#job-dialog-wcet").val()
				deadline = $("#job-dialog-deadline").val()
				$("##{jobId}").data("name", name)
				$("##{jobId}").data("wcet", parseInt(wcet))
				$("##{jobId}").data("deadline", parseInt(deadline))
				# TODO Create a method for updating the visual representation of a job...
				$("##{jobId}").html(name + "<br><small>&lt;" + wcet + "," + deadline + "&gt;</small>")
				# TODO Support handling type changes...
				$(this).dialog("close")
			"Cancel": ->
				$(this).dialog("close")			
			"Delete": ->
				alert("Not supported yet")	#TODO Implement this!!
				$(this).dialog("close")

# Edits an edge
@dirt.editEdge = (connection) ->
	$("#edge-dialog-delay").val(connection.delay)
	$("#edge-dialog").dialog
		modal: true
		resizable: false
		closeText: "cancel"
		draggable: false
		buttons:
			"Apply": ->
				inspect(connection)
				connection.delay = parseInt($("#edge-dialog-delay").val())
				jsPlumb.repaint(connection.sourceId)
				$(this).dialog("close")
			"Cancel": ->
				$(this).dialog("close")
			"Delete": ->
				jsPlumb.detach(connection.source, connection.target)
				$(this).dialog("close")
	$("#edge-dialog").keyup (e) ->
		if e.keyCode == 13
			alert "saved!"	#TODO Handle keycode saving...

# Converts a name to an ID
@dirt.name2id = (name) -> return "job_#{name}"

# Adds a new job to the document
@dirt.addJob = (name, wcet = 0, deadline = 1, type = drt.Job.Types.Job, x = 0, y = 0) ->
	job = document.createElement("div")
	job.id = dirt.name2id(name)
	job.innerHTML = name
	$(job).addClass("job no-select")
	if type == drt.Job.Types.Fork
		$(job).addClass("fork")
	else if type == drt.Job.Types.Merge
		$(job).addClass("merge")

	# add to DOM and set coordinates
	$("#document").append(job)
	$(job).css("left",(x - ($(job).width() - 10)) + "px")
	$(job).css("top",(y - parseInt($(job).height() * 4)) + "px")
	job.innerHTML += "<br><small>&lt;" + wcet + "," + deadline + "&gt;</small>"

	# add job to jsPlumb
	ep = jsPlumb.addEndpoint(job, { anchor: "Center", isSource: true, isTarget: true })
	# assign job data
	$(job).data("endpoint", ep)
	$(job).data("name", 	name)
	$(job).data("type", 	type)
	$(job).data("wcet", 	wcet)
	$(job).data("deadline", deadline)
	$(job).bind "dblclick", ->
		dirt.editJob(job.id)
	jsPlumb.draggable($("#"+job.id))

# Clear the document
@dirt.clear = ->
	dirt.title = "untitled"
	jsPlumb.deleteEveryEndpoint()
	$("#document").empty()
	return

# Create an edge from job named source to job named target with inter-release time delay
@dirt.addEdge = (src, dest, delay) ->
	conn = jsPlumb.connect({"source": $("#" + dirt.name2id(src)).data("endpoint"), "target": $("#" + dirt.name2id(dest)).data("endpoint")})
	conn.delay = delay
	jsPlumb.repaint(dirt.name2id(src))
	return

# Read file
@dirt.readFile = ->
	# HTML5, FileReader document at: http://www.html5rocks.com/en/tutorials/file/dndfiles/
	files = document.getElementById("file-dialog-file").files
	if files.length > 0
		reader = new window.FileReader()
		reader.onloadend = (args) =>
			if(args.target.readyState == FileReader.DONE)
				if dirt.loadXml(args.target.result)
					$.gritter.add
								title: "Loaded task from file"
								text: "dirt successfully loaded task \"#{dirt.name}\" from file."
		reader.readAsText(files[0])
		$("#file-dialog").dialog("close")

# Load Xml to document
@dirt.loadXml = (xml) ->
	tasks = drt.parseXml(xml)
	if tasks.length == 0
		$("#error-dialog").append("<b>No tasks found in Xml document</b><br/>")
		$("#error-dialog").dialog(modal: true, resizable: false, draggable: false)
		return false
	if tasks.length > 1
		alert "This editor only supports DRTs with one task, loading task \"#{tasks[0].name}\""
	task = tasks[0]
	if dirt.validate(task)
		dirt.loadTask(task)
		return true
	return false
		
# Load the DRT into the editor
@dirt.loadTask = (task) ->
	# Clear document
	dirt.clear()
	# Create jobs
	for job in task.jobs
		dirt.addJob(job.name, job.wcet, job.deadline, job.type, job.x, job.y)
	# Create all the edges
	for source in task.jobs
		for target in task.jobs when task.edge(source, target)
			dirt.addEdge(source.name, target.name, task.delay(source, target))
	dirt.title = task.name

# Validate a task and show errors in dialog
@dirt.validate = (task) ->
	# Clear error dialog
	$("#error-dialog").empty()
	$("#error-dialog").append("<b>Errors in \"#{task.name}\":</b><br/>")
	# Validate, show errors if failed
	unless drt.validate(task, (msg) ->	$("#error-dialog").append("#{msg}<br/>"))
		# Show error dialog, validate prints errors to this dialog
		$("#error-dialog").dialog(modal: true, resizable: false, draggable: false)
		return false
	return true

# Title of the current task (in the editor)
@dirt.title = "untitled"		#TODO Do something smart with the title head/title should reflect it, and you should be able to change it

# Get task from editor (in-memory-format)
@dirt.getTask = ->
	#Create a list of all jobs
	jobs = []
	$("#document .job").each ->
		name = $(this).data("name")
		wcet = $(this).data("wcet")
		deadline = $(this).data("deadline")
		type = drt.Job.Types.Job
		pos = $(this).position() #TODO Figure out if we do the position stuff right everywhere
		if $(this).hasClass("fork")
			type = drt.Job.Types.Fork
		if $(this).hasClass("merge")
			type = drt.Job.Types.Merge
		jobs.push(new drt.Job(name, wcet, deadline, type, pos.left, pos.top))
	task = new drt.Task(dirt.title, jobs)
	$("#document .job").each ->
		source = $(this).data("name")
		id = task.id(source)
		ep = $(this).data("endpoint")
		for conn in ep.connections when conn.sourceId == dirt.name2id(source)
			target = $("#" + conn.targetId).data("name")
			task.addEdge(id, task.id(target), conn.delay)
	return task

# Populate analysis with something interesting
dirt.analyze = ->
	task = dirt.getTask()
	if dirt.validate(task)
		U = drt.utilization(task)
		$("#analysis").append("<br><br><h1 style='padding-left: 80px;'>Utilization: #{U}</h1>")
		$.gritter.add
				title: "Task utilization computed"
				text: "Utilization of task \"#{dirt.name}\" was computed to #{U}."

