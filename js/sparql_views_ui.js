// SPARQL Views UI
// lin.w.clark@gmail.com
(function($) {
  /**
	* Class: termBox
	* A box that can
  */
	var termBox = function (termPosition, id) {
		return {
			getTid : function() {
				return termPosition + "_" + id;
			},

			setEndpoints : function(leftEndpoint, rightEndpoint) {
				this.endpoints = new Object();
				this.endpoints['left'] = leftEndpoint;
				this.endpoints['right'] = rightEndpoint;
			},

			getEndpoints : function(endpoint) {
				return this.endpoints;
			},

			create : function() {
				var box = $("<div class='box'></div>");
				var headerBar = $("<div class='header-bar'><span class='detach'>X</span><span class='minimize'>&ndash;</span></div>");

				var nodeTypeSwitcher = "<input type='checkbox' class='variable' name='" + termPosition + "_type_" + id + "' value='variable'/>"
					+ "<label for='" + termPosition + "_type_" + id + "'>Get as variable</label>&nbsp;"
					+ "<input type='checkbox' class='value' name='" + termPosition + "_type_" + id + "' value='value'/>"
					+ "<label for='" + termPosition + "_type_" + id + "'>Get by value</label><br>";

				if (termPosition == 'predicate') {
					box
						.prepend(headerBar.clone().attr("rel", this.getTid()))
						.addClass("predicate")
						.attr("id", this.getTid())
						.hide();
				}
				else {
					box
						.html(nodeTypeSwitcher)
						.prepend(headerBar.clone().attr("rel", this.getTid()))
						.addClass("node")
						.attr("id", this.getTid())
						.hide();
				}
				$('#workspace').append(box);
				return this;
			},

			addForm : function() {
				var nodeForm = function(type) {
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
					form.attr("id", termPosition + "_" + type + "_" + id);

					return form;
				};

				var activateSwitcher = function(type) {
				  var otherType = (type == 'variable') ? "value" : "variable";
					$("#" + termPosition + "_" + id + " ." + type).click(function () {
    		    if ($("input[name=" + termPosition + "_type_" + id +"][value=" + otherType + "]").is(':checked')) {
              $("input[name=" + termPosition + "_type_" + id +"][value=" + otherType + "]").attr('checked', false);
						  $("#" + termPosition + "_" + otherType + "_" + id).toggle(500);
    			  }
    			  $("#" + termPosition + "_" + type + "_" + id).toggle(500);
    		  });
				}

				nodeForm('variable').hide().appendTo($('#' + this.getTid()));
				nodeForm('value').hide().appendTo($('#' + this.getTid()));
				activateSwitcher('variable');
				activateSwitcher('value');
			},

			position : function(top, left) {
				$('#' + this.getTid()).css({
    	    position: 'absolute',
					top: top,
					left: left
        });
				return this;
			},

			addEndpoints : function() {
				var termType = (termPosition == 'predicate') ? 'predicate' : 'node';

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

				var endpoint = function(termType, endpointPosition) {
					switch (endpointPosition) {
						case 'left':
							var anchorPosition = jsPlumb.Anchors.LeftMiddle;
							var scope = (termType == 'node') ? "objectEndpoint" : "predicateSubjectEndpoint";
							var dropScope = (termType == 'node') ? "predicateObjectEndpoint" : "subjectEndpoint";
							break;
						case 'right':
							var anchorPosition = jsPlumb.Anchors.RightMiddle;
							var scope = (termType == 'node') ? "subjectEndpoint" : "predicateObjectEndpoint";
							var dropScope = (termType == 'node') ? "predicateSubjectEndpoint" : "objectEndpoint";
							break;
					}

					var maxConnections = (scope == "subjectEndpoint") ? 100 : 1;

					return {
						endpoint:new jsPlumb.Endpoints.Dot({radius:9}),
						anchor: anchorPosition,
						scope: scope,
						style:{ strokeStyle: '#A2CD3A' },
						maxConnections: maxConnections,
						isSource:true,
						dragOptions : scope,
						isTarget:true,
						dropOptions : $.extend({scope:dropScope}, dropOptions),
						connectorStyle:{ strokeStyle:'#ccc', lineWidth:3 },
						connector: new jsPlumb.Connectors.Bezier(63)
					};
				}

				leftEndpoint = $('#' + this.getTid()).addEndpoint(endpoint(termType, 'left'));
				rightEndpoint = $('#' + this.getTid()).addEndpoint(endpoint(termType, 'right'));
				leftEndpoint.canvas.style.display = 'none';
				rightEndpoint.canvas.style.display = 'none';
				$(leftEndpoint.canvas).fadeIn(1000);
				$(rightEndpoint.canvas).fadeIn(1000);
				this.setEndpoints(leftEndpoint, rightEndpoint);
			}
		}
	};

	function _addBoxes(ui) {
		// ID variables.
		var id = Math.floor(Math.random()*999999);
		var pid = "predicate_" + id;
		var sid = "subject_" + id;
		var oid = "object_" + id;

		// Layout variables.
		var width = 200;
		var top = ui.position['top'];
		var left = ui.position['left'];
		var boxMargin = 100;

		// Create and position boxes.
		s = termBox('subject', id).create().position(top, left - width - boxMargin);
		p = termBox('predicate', id).create().position(top, left);
		o = termBox('object', id).create().position(top, left + width + boxMargin);

		predicateBox = $('#' + pid)
			.append(ui.draggable.text())
			.attr("dataset-triplevalue", ui.draggable.text());
		subjectBox = $('#' + sid);
		objectBox = $('#' + oid);

		predicateBox.fadeIn(1000, function(){
			// Add endpoints to the boxes.
			p.addEndpoints();
			subjectBox.fadeIn(1000);
			s.addEndpoints();
			objectBox.fadeIn(1000);
			o.addEndpoints();

			// Add the connections between the predicate and the nodes.
			sConnection = jsPlumb.connect({ source:s.getTid(), target:p.getTid(), sourceEndpoint:s.getEndpoints().right, targetEndpoint:p.getEndpoints().left});
			sConnection.canvas.style.display = 'none';
			oConnection = jsPlumb.connect({ source:o.getTid(), target:p.getTid(), sourceEndpoint:o.getEndpoints().left, targetEndpoint:p.getEndpoints().right});
			oConnection.canvas.style.display = 'none';

			$(sConnection.canvas).fadeIn(500);
			$(oConnection.canvas).fadeIn(500);
			
			$(".detach").click(function() {
				jsPlumb.detachAll($(this).parent().attr("rel"));
				jsPlumb.removeAllEndpoints($(this).parent().attr("rel"));
				$(this).parent().parent().remove();
			});
			
			// Add the forms. We don't do this at first because the extra height
			// would make the endpoints center weirdly.
			s.addForm();
			o.addForm();
		});
	}
				
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
			
			addBoxes : function(ui) {
				_addBoxes(ui);
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
    sparqlViews.processSparql();
  }

	$.fn.addTriple = function(ui) {
    sparqlViews.addBoxes(ui);
  }
})(jQuery);

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
				      $.prototype.addTriple(ui);
					  }
				  });
			  },
			  error: function(xhr, textStatus, errorThrown) {
			    alert('An error occurred ' + (errorThrown ? errorThrown : xhr.status));
		    }
	    });
    });

		$(".process").click(function() {
      $.prototype.processSparql();
    });

    $("#clear").click(function() { jsPlumb.detachEverything(); });
  });
	/* end jQuery */
