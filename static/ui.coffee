@dirt ?= {}

@dirt.main = =>
	$("#add-job").button()
	$("#add-edge").button()
	$("#open").button()
	$("#mode").buttonset()

	$(".widget").addClass("no-select")

	# Initial jsPlumb canvas
	jsPlumb.ready(dirt.initJsPlumb)

	# Make sure we clean up after jsPlumb
	$(window).unload -> jsPlumb.unload()

	# Open file when file dialog file is changed
	#$("#file-dialog-file").change(@dirt.loadInput)
	document.getElementById("file-dialog-file").addEventListener("change", @dirt.readFile, false)

	# Dialog stuff open file
	$("#open").click =>
		@dirt.loadXml """
					<drt>
						<task name="task0">
							<job name="v1" wcet="2" deadline="3"/>
							<job name="v2" wcet="2" deadline="5"/>
							<job name="v3" wcet="3" deadline="4"/>
							<edge source="v1" destination="v2" delay="4"/>
							<edge source="v2" destination="v3" delay="5"/>
							<edge source="v3" destination="v1" delay="6"/>
						</task>
					</drt>
				"""
		return false
		unless window.File and window.FileReader and window.FileList
			alert("Browser support lacking")
			return
		$("#file-dialog").dialog
								modal: true
								resizable: false
								closeText: "cancel"
								draggable: false
# Init JsPlumb stuff
@dirt.initJsPlumb = ->
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
		["Label", { location: 0.5, label: (ci) -> return ci.connection.delay || "" , 
		cssClass:"label no-select"}]
	]

	jsPlumb.bind "jsPlumbConnection", (connInfo) -> 
		src = $("#" + connInfo.sourceId).data("endpoint")
		end = src = $("#" + connInfo.targetId).data("endpoint")
		connInfo.connection.delay = ""+Math.round(Math.random()*100) #TODO Provide default delay
		count = 0
		for conn in src.connections
			if (conn.targetId == connInfo.targetId and 
				conn.sourceId == connInfo.sourceId)
					count = count+1

			if (count > 1)
				src.detach(connInfo.connection)
	# Click handler for adding jobs
	$("#add-job").click -> 
		dirt.addJob("j" + dirt.getNextJobName()) 

# TODO Event handler for modifying/deleting a job (check if new job exists!)
# TODO Event handler for modifying/deleting an edge

@dirt.getNextJobName = ->
	number = 0
	number = number + 1 while document.getElementById("job_" + "j" + number)
	return number

# Adds a new job to the document
@dirt.addJob = (name, wcet = 0, deadline = 1, type = drt.Job.Types.Job, x = 0, y = 0) ->
	#TODO Append wcet, deadline, and draw different background image if type != Job, also set x, y
	job = document.createElement("div")
	job.id = "job_" + name		# TODO Create a function for converting name to id, and use it everywhere
	job.innerHTML = name
	$(job).addClass("job")
	$("#document").append(job)
	ep = jsPlumb.addEndpoint(job, { anchor: "Center", isSource: true, isTarget: true })
	$(job).data("endpoint", ep)
	jsPlumb.draggable(job.id)

# Clear the document
@dirt.clear = ->
	# TODO Check if this is right...
	jsPlumb.deleteEveryEndpoint()
	$("#document").empty()
	return

# Create an edge from job named source to job named target with inter-release time delay
@dirt.addEdge = (source, target, delay) ->
	conn = jsPlumb.connect({"source": "job_" + source, "target": "job_" + target})
	conn.delay = "#{delay}"
	jsPlumb.repaint("job_" + source)
	return

# Read file
@dirt.readFile = ->
	# HTML5, FileReader document at: http://www.html5rocks.com/en/tutorials/file/dndfiles/
	files = document.getElementById("file-dialog-file").files
	if files.length > 0
		reader = new window.FileReader()
		reader.onloadend = (args) =>
			if(args.target.readyState == FileReader.DONE)
				dirt.loadXml(args.target.result)
		reader.readAsText(files[0])
		$("#file-dialog").dialog("close")

# Load Xml to document
@dirt.loadXml = (xml) ->
	tasks = drt.parseXml(xml)
	errors = false
	$("#error-dialog").empty()
	for task in tasks
		$("#error-dialog").append("<b>Errors in \"#{task.name}\":</b><br/>")
		errors = true unless drt.validate(task, (msg) -> $("#error-dialog").append("#{msg}<br/>"))
	if tasks.length == 0
		$("#error-dialog").append("<b>No tasks found in Xml document</b><br/>")
		errors = true
	if errors
		# Show error dialog, validate prints errors to this dialog
		$("#error-dialog").dialog(modal: true, resizable: false, draggable: false)
	else
		#Load the DRT into the editor
		if tasks.length > 1
			alert "This editor only supports DRTs with one task, loading task \"#{tasks[0].name}\""
		# Clear document
		dirt.clear()
		task = tasks[0]
		# Create jobs
		for job in task.jobs
			dirt.addJob(job.name, job.wcet, job.deadline, job.type, job.x, job.y)
		# Create all the edges
		for source in task.jobs
			for target in task.jobs when task.edge(source, target)
				dirt.addEdge(source.name, target.name, task.delay(source, target))





