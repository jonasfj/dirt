@dirt ?= {}

@dirt.main = =>
	# $("#add-job").button()
	$("#jobs").buttonset()
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
		["Label", { location: 0.5, label: (connInfo) -> return connInfo.connection.labelText || "" , 
		cssClass:"label no-select"}]
	]

	jsPlumb.bind "jsPlumbConnection", (connInfo) -> 
		src = $("#" + connInfo.sourceId).data("endpoint")
		end = src = $("#" + connInfo.targetId).data("endpoint")
		connInfo.connection.labelText = ""+Math.round(Math.random()*100)
		count = 0
		for conn in src.connections
			if (conn.targetId == connInfo.targetId and 
				conn.sourceId == connInfo.sourceId)
					count = count+1

			if (count > 1)
				src.detach(connInfo.connection)
	# updates current pos of mouse
	$(document).mousemove (e) -> 
		dirt.mouseX = e.pageX;
		dirt.mouseY = e.pageY;

	# Click handler for adding jobs
	$("#add-job").click -> 
		dirt.adding = true

	# TODO: pass correct 'type' to addJob()
	$("#document").click ->
		if dirt.adding
			dirt.addJob("j" + dirt.getNextId(), "job", dirt.mouseX, dirt.mouseY) 
			dirt.adding = false

@dirt.mouseX = 0
@dirt.mouseY = 0
# bool whether job is being added
@dirt.adding = false
@dirt.nextId = 0
@dirt.getNextId = ->
	id = dirt.nextId
	dirt.nextId = dirt.nextId + 1
	return id

# Adds a new job to the document
@dirt.addJob = (name, type, x, y) ->
	job = document.createElement("div")
	job.id = name
	job.innerHTML = job.id
	$(job).addClass("job")
	$("#document").append(job)
	# Not pretty, but it works...
	$(job).css("left",(x-($(job).width()-10))+"px")
	$(job).css("top",(y-parseInt($(job).height()*4))+"px")
	ep = jsPlumb.addEndpoint(job, { anchor: "Center", isSource: true, isTarget: true })
	$(job).data("endpoint", ep)
	jsPlumb.draggable(job.id)

# Read file
@dirt.readFile = =>
	# HTML5, FileReader document at: http://www.html5rocks.com/en/tutorials/file/dndfiles/
	files = document.getElementById("file-dialog-file").files
	if files.length > 0
		reader = new window.FileReader()
		reader.onloadend = (args) =>
			if(args.target.readyState == FileReader.DONE)
				@dirt.loadXml(args.target.result)
		reader.readAsText(files[0])
		$("#file-dialog").dialog("close")

@dirt.loadXml = (xml) =>
	tasks = @drt.parseXml(xml)
	errors = false
	for task in tasks
		unless @drt.validate(task, (msg) => $("#error-log").append("#{msg}<br/>"))
			alert("Error in task: #{task.name}")
			errors = true
	unless errors
		alert("We've validated #{tasks.length} tasks")
	#TODO Load the DRT into the editor!!!


