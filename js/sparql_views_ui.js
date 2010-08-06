// SPARQL Views UI
// lin.w.clark@gmail.com
(function() {
  $(document).ready(function() {
		// We do not want to have predicates connected to more than one object.
    // Because jsPlumb maxConnections only checks whether this endpoint is
    // full from the source perspective and not the target perspective, we
    // have to test during the drag itself.
    $("._jsPlumb_endpoint").bind("dragstart", function( event ){
       newEvent = true;
    })
    .bind("drag", function(event, ui) {
    // @todo If issue isn't fixed in jsPlumb, set a class on the endpoint
    // to register which item it is attached to. Then run through all
    // connections to see whether that item is already connected.
    	id = event.target.id;
    	if (newEvent == true && !$(this).hasClass("predicate-subject")) {
    	sparqlViews.setDroppable(id);
    		/*connectedEndpoints = _getConnectedEndpoints();
    		$(".dragActive").each(function(index) {
    			if (connectedEndpoints[$(this).attr("id")]) {
    				$(this).droppable( "option", "disabled", true );
    				$(this).removeClass("dragActive");
    			}
    		});*/
      }
      newEvent = false;
    })
				
    $(".hide").click(function() {
      jsPlumb.toggle($(this).attr("rel"));
    });

    $("#dataset").click(function() {
      $.ajax({
    	  type: 'GET',
    		url: "/sparql-views/get-predicates",
    		dataType: 'html',
    		success: function(html, textStatus) {
    		  $('#predicate-store').append(html);
    			$('.predicate').draggable({
			      helper: "clone"
		      });
    			$('#workspace').droppable({
    			  accept: '.predicate',
    				drop: function(event,ui) {
				      addTriple(ui);
					  }
				  });
			  },
			  error: function(xhr, textStatus, errorThrown) {
			    alert('An error occurred ' + (errorThrown ? errorThrown : xhr.status));
		    }
	    });
    });

		$(".process").click(function() {
      sparqlViews.processSparql();
    });

    $("#clear").click(function() { jsPlumb.detachEverything(); });

    originalEndpointClass = jsPlumb.endpointClass;
    jsPlumb.Defaults.DragOptions = { cursor: 'pointer', zIndex:2000 };
    jsPlumb.Defaults.PaintStyle = { strokeStyle:'#666' };
    jsPlumb.Defaults.EndpointStyle = { width:20, height:16, strokeStyle:'#666' };
    jsPlumb.Defaults.Endpoint = new jsPlumb.Endpoints.Rectangle();
    jsPlumb.Defaults.Anchors = [jsPlumb.Anchors.TopCenter, jsPlumb.Anchors.TopCenter];

    var dropOptions = {
      tolerance:'touch',
      hoverClass:'dropHover',
      activeClass:'dragActive'
    };

    var subjectEndpoint = {
      endpoint:new jsPlumb.Endpoints.Dot({radius:9}),
      anchor: jsPlumb.Anchors.RightMiddle,
      scope: "subjectEndpoint",
      style:{ strokeStyle: '#A2CD3A' },
      maxConnections:100,
      // This can't be the source because maxConnections is not checked in both directions. If this is fixed in jsPlumb (most likely in initDraggable), then this can go in both directions.
      isSource:true, 
      dragOptions : { scope:'subjectConnection' },
      isTarget:true,
      dropOptions : $.extend({scope:'predicateSubjectConnection'}, dropOptions),
      connectorStyle:{ strokeStyle:'#ccc', lineWidth:3 },
      connector: new jsPlumb.Connectors.Bezier(63),
    };
    var predicateSubjectEndpoint = {
      endpoint:new jsPlumb.Endpoints.Dot({radius:9}),
			anchor: jsPlumb.Anchors.LeftMiddle,
			scope: "predicateSubjectEndpoint",
      style:{ strokeStyle: '#A2CD3A' },
      maxConnections:1,
      isSource:true,
      dragOptions : { scope:'predicateSubjectConnection' },
      isTarget:true,
      dropOptions : $.extend({scope:'subjectConnection'}, dropOptions),
      connectorStyle:{ strokeStyle:'#ccc', lineWidth:3 },
      connector: new jsPlumb.Connectors.Bezier(63),
    };
    var objectEndpoint = {
      endpoint:new jsPlumb.Endpoints.Dot({radius:9}),
			anchor: jsPlumb.Anchors.LeftMiddle,
			scope: "objectEndpoint",
      style:{ strokeStyle:'#A9529F' },
      maxConnections:1,
      // This can't be the source because maxConnections is not checked in both directions. If this is fixed in jsPlumb (most likely in initDraggable), then this can go in both directions.
      isSource:true, 
      dragOptions : { scope:'objectConnection' },
      isTarget:true,
      dropOptions : $.extend({scope:'predicateObjectConnection'}, dropOptions),
      connectorStyle:{ strokeStyle:'#ccc', lineWidth:3 },
      connector: new jsPlumb.Connectors.Bezier(63),
    };
    var predicateObjectEndpoint = {
      endpoint:new jsPlumb.Endpoints.Dot({radius:9}),
			anchor: jsPlumb.Anchors.RightMiddle,
			scope: "predicateObjectEndpoint",
      style:{ strokeStyle:'#A9529F' },
      maxConnections:1,
      isSource:true,
      dragOptions : { scope:'predicateObjectConnection' },
      isTarget:true,
      dropOptions : $.extend({scope:'objectConnection'}, dropOptions),
      connectorStyle:{ strokeStyle:'#ccc', lineWidth:3 },
      connector: new jsPlumb.Connectors.Bezier(63),
    };

    function addTriple(ui) {
      // ID variables.
    	var id = Math.floor(Math.random()*999999);
    	var pid = "predicate_" + id;
    	var sid = "subject_" + id;
    	var oid = "object_" + id;

    	// Layout variables.
    	var top = ui.position['top'];
    	var left = ui.position['left'];
    	var boxMargin = 100;

      // Object variables.
      var workspace = $('#workspace');
      var predicateBox = $("<div class='box'></div>");
    	var subjectBox = predicateBox.clone();
    	var objectBox = predicateBox.clone();
			var headerBar = $("<div class='header-bar'><span class='detach'>X</span><span class='minimize'>&ndash;</span></div>");

    	var _getNodeTypeSwitcher = function(position, id) {
				return "<input type='checkbox' class='variable' name='" + position + "_type_" + id + "' value='variable'/>"
				  + "<label for='" + position + "_type_" + id + "'>Get as variable</label>&nbsp;"
    	    + "<input type='checkbox' class='value' name='" + position + "_type_" + id + "' value='value'/>"
    	    + "<label for='" + position + "_type_" + id + "'>Get by value</label><br>";
			}
			
			subjectNodeTypeSwitcher = _getNodeTypeSwitcher('subject', id);
			objectNodeTypeSwitcher = _getNodeTypeSwitcher('object', id);
    
      predicateBox
    	  .text(ui.draggable.text())
				.prepend(headerBar.clone().attr("rel", pid))
    	  .addClass("predicate")
    		.attr("id", pid)
    		.attr("dataset-triplevalue", ui.draggable.text())
    	  .hide();
      subjectBox
    	  .html(subjectNodeTypeSwitcher)
				.prepend(headerBar.clone().attr("rel", sid))
        .addClass("node")
    		.attr("id", sid)
    		.hide();
    	objectBox
    	  .html(objectNodeTypeSwitcher)
				.prepend(headerBar.clone().attr("rel", oid))
        .addClass("node")
    		.attr("id", oid)
    		.hide();
      workspace.append(predicateBox);
    	workspace.append(subjectBox);
    	workspace.append(objectBox);
    
    	predicateBox.css({
    	  position: 'absolute',
    	  top: top,
    	  left: left
      });
    	subjectBox.css({
    	  position: 'absolute',
        top: top,
    		left: left - subjectBox.width() - boxMargin
      });
    	objectBox.css({
    	  position: 'absolute',
        top: top,
    		left: left + predicateBox.width() + boxMargin
      });
    	
      predicateBox.fadeIn(1000, function(){
				// Add endpoints to the boxes.
    	  pLeftEndpoint = $('#' + pid).addEndpoint(predicateSubjectEndpoint);
				pRightEndpoint = $('#' + pid).addEndpoint(predicateObjectEndpoint);
				sLeftEndpoint = $('#' + sid).addEndpoint(objectEndpoint);
				sRightEndpoint = $('#' + sid).addEndpoint(subjectEndpoint);
				oRightEndpoint = $('#' + oid).addEndpoint(subjectEndpoint);
				oLeftEndpoint = $('#' + oid).addEndpoint(objectEndpoint);
    		pLeftEndpoint.canvas.style.display = 'none';
    		pRightEndpoint.canvas.style.display = 'none';
				sLeftEndpoint.canvas.style.display = 'none';
    		sRightEndpoint.canvas.style.display = 'none';
				oLeftEndpoint.canvas.style.display = 'none';
        oRightEndpoint.canvas.style.display = 'none';

    		subjectBox.fadeIn(1000);
    		$(pLeftEndpoint.canvas).fadeIn(1000);
        objectBox.fadeIn(1000);
    		$(pRightEndpoint.canvas).fadeIn(1000);

    		sConnection = jsPlumb.connect({ source:sid, target:pid, sourceEndpoint:sRightEndpoint, targetEndpoint:pLeftEndpoint});
    		sConnection.canvas.style.display = 'none';
    		oConnection = jsPlumb.connect({ source:oid, target:pid, sourceEndpoint:oLeftEndpoint, targetEndpoint:pRightEndpoint});
    		oConnection.canvas.style.display = 'none';
    		
    		$(sRightEndpoint.canvas).fadeIn(1000, function() {
          $(sConnection.canvas).fadeIn(500);
        });
    		$(oRightEndpoint.canvas).fadeIn(1000);
    		$(sLeftEndpoint.canvas).fadeIn(1000);
    		$(oLeftEndpoint.canvas).fadeIn(1000, function() {
          $(oConnection.canvas).fadeIn(500);
        });
    		
				$(".detach").click(function() {
				  jsPlumb.detachAll($(this).parent().attr("rel"));
			    jsPlumb.removeAllEndpoints($(this).parent().attr("rel"));
			    $(this).parent().parent().remove();
        });
				
				var _getNodeForm = function(position, type, id) {
				  var prependChar = '';
					var appendChar = '';
				  var form = $("<div></div>");

					if (type == "variable") {
				    prependChar = "?";
						appendChar = '';
					}
					else if (type == "value") {
				    prependChar = '"';
						appendChar = '"';
					}

					form.html(prependChar + " <input type='text' name='" + type + "_value'/> " + appendChar);
					form.attr("id", position + "_" + type + "_" + id);
					
					return form;
				}
    		
				_getNodeForm('subject', 'variable', id).hide().appendTo(subjectBox);
				_getNodeForm('subject', 'value', id).hide().appendTo(subjectBox);
				_getNodeForm('object', 'variable', id).hide().appendTo(objectBox);
				_getNodeForm('object', 'value', id).hide().appendTo(objectBox);
				
				var _activateSwitcher = function(position, type, id) {
				  var otherType = (type == 'variable') ? "value" : "variable";
					$("#" + position + "_" + id + " ." + type).click(function () {
    		    if ($("input[name=" + position + "_type_" + id +"][value=" + otherType + "]").is(':checked')) {
              $("input[name=" + position + "_type_" + id +"][value=" + otherType + "]").attr('checked', false);
						  $("#" + position + "_" + otherType + "_" + id).toggle(500);
    			  }
    			  $("#" + position + "_" + type + "_" + id).toggle(500);
    		  });
				}
				
				_activateSwitcher('subject', 'variable', id);
				_activateSwitcher('subject', 'value', id);
				_activateSwitcher('object', 'variable', id);
				_activateSwitcher('object', 'value', id);
    	});
    }

  });
				
  function _getTriples() {
    var sparqlQuery = (sparqlQuery != undefined) ? sparqlQuery : '';
	  var triples = new Object();
    // Add the subject.
    // Get all the subjectEndpoints on this subject.
	  var connections = jsPlumb.getConnections();

	  // Get all of the subject values and store the target predicates as an array.
	  for (i in connections['subjectEndpoint']) {
	    var connection = connections['subjectEndpoint'][i];
		  var subjectLocation = "sourceId";
		  var predicateLocation = "targetId";
		  _addTriple(connection, subjectLocation, predicateLocation, triples);
	  }
	  for (i in connections['predicateSubjectEndpoint']) {
	    var connection = connections['predicateSubjectEndpoint'][i];
		  var subjectLocation = "targetId";
		  var predicateLocation = "sourceId";
		  _addTriple(connection, subjectLocation, predicateLocation, triples);
	  }
	
	  return triples;
  }

  function _addTriple(connection, subjectLocation, predicateLocation, triples) {
    var subjectId = connection[subjectLocation];
    var predicateId = connection[predicateLocation];
    subject = _getNodeValue(subjectId);
	  predicate = $('#' + predicateId).attr("dataset-triplevalue");
	  object = _getObject(connection, predicateId);

	  // If there is a full triple, add this triple to the subject's array.
	  if (object) {
	    if (triples[subject] == undefined) {
        triples[subject] = new Array();
	    }
	    triples[subject].push(new Array(predicate, object));
	  }
  }

  function _getObject(connection, predicateId) {
		var object = null;
		var options = [];
		options.source = predicateId;
		options.scope = "predicateObjectEndpoint";
		var objectConnection = jsPlumb.getConnections(options);
		
		if (objectConnection["predicateObjectEndpoint"] && objectConnection["predicateObjectEndpoint"][0]) {
		  objectId = objectConnection["predicateObjectEndpoint"][0].targetId;
			object = $('#' + objectId).attr("dataset-triplevalue");
		}
		else {
		  options = [];
			options.target = predicateId;
      options.scope = "objectEndpoint";
			objectConnection = jsPlumb.getConnections(options);
			if (objectConnection["objectEndpoint"] && objectConnection["objectEndpoint"][0]) {
			  objectId = objectConnection["objectEndpoint"][0].sourceId;
			  object = _getNodeValue(objectId);
			}
		}
		
		return object;
  }
	
	function _getNodeValue(nid) {
    var nodeValue = null;
    if ($("#" + nid + " input[value=variable]").is(':checked')) {
      nodeValue = "?" + $("#" + nid + " input[name=variable_value]").val();
		}
		else if ($("#" + nid + " input[value=value]").is(':checked')) {
      nodeValue = "\"" + $("#" + nid + " input[name=value_value]").val() + "\"";
		}
    else {
      nodeValue = $('#' + nid).attr("dataset-triplevalue");
		}
		return nodeValue;
	}
  
   window.jsPlumb.CurrentLibrary.initDroppable = function(el, options) {
      options['scope'] = options['scope'] || jsPlumb.Defaults.Scope;
      options['accept'] = '._jsPlumb_endpoint';
      el.droppable(options);
   };   
   
  /*
   Class: sparqlViews
   
   This is the main entry point to sparqlViews.  Static methods are used instead of methods in jQuery's "$.fn" object itself, to stay in line with jsPlumb, which this depends on.  
   */
    var sparqlViews = window.sparqlViews = {
      processSparql : function() {
				var sparqlQuery = (sparqlQuery != undefined) ? sparqlQuery : 'SELECT * WHERE {\n';
				var triples = new Array();
				triples = _getTriples();
				for (tripleSubject in triples) {
								if (triples[tripleSubject].length == 1) {
												triple = tripleSubject + " " + triples[tripleSubject][0].join(" ");
								}
								else if (triples[tripleSubject].length > 1) {
												triple = tripleSubject + " ";
												for (i = 0; i < triples[tripleSubject].length; i++) {
													triple += triples[tripleSubject][i].join(" ");
													if (i+1 == triples[tripleSubject].length) {
													   triple += " .\n"
													}
													else {
													   triple += " ;\n"
													}
												}
								}
								sparqlQuery += triple + " .\n";
				}
				sparqlQuery += '}';
				$("#edit-query").html(sparqlQuery);
      },
			
			setDroppable : function(id) {
			  // @todo If issue isn't fixed in jsPlumb, set a class on the endpoint
				// to register which item it is attached to. Then run through all
				// connections to see whether that item is already connected.
			
			  /*connections = jsPlumb.getConnections();
				for (scope in connections) {
				  for (i in connections[scope]) {
					  window.console.log(connections[scope][i]);
					}
				}*/
			}
  };
})(jQuery);


//jQuery plugin code
(function($){
  $.fn.processSparql = function() {
  return this.each(function() 
  {
    var id = $(this).attr("id");
    sparqlViews.processSparql(id);
  });
  }
})(jQuery);
