$(function(){
	// Create buttons
	$("#add-job").button();
	$("#add-edge").button();
	$("#mode").buttonset();

	$(".widget").addClass("no-select");

	// Initial jsPlumb canvas
	jsPlumb.ready(function() {
		// Set container for the document
		jsPlumb.Defaults.Container = $("#document");

		// Create add job action
		$("#add-job").click(function(){
			var job = document.createElement("div");
			$(job).addClass("job");
			$("#document").append(job);
			jsPlumb.addEndpoint(job);
		});
	});

	// Make sure we clean up after jsPlumb
	$(window).unload(function(){ jsPlumb.unload(); });
});
