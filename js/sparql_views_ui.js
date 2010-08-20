// SPARQL Views UI
// lin.w.clark@gmail.com
(function($) {
	/**
	* Class: queryBuilder
	* The queryBuilder is instantiated when the window opens.
  */
	var queryBuilder = function () {
		return {
			addBoxes : function(text, position, sid) {
				// ID variables.
				var id = Math.floor(Math.random()*999999);

				// Layout variables.
				var width = 200;
				var top = position['top'];
				var left = position['left'];
				var boxMargin = 100;

				// Create and position boxes. If this isn't being created from an exisitng
				// subject, create a new subject.
				if (sid == null) {
					s = termBox('subject', id).create().position(top, left - width - boxMargin);
					subjectBox = $('#' + s.getTid());
				}
				else {
					var s = new Object;
					s.getTid = function() {
						return sid;
					}
					s.getEndpoints = function() {
						return endpoints[sid];
					}
				}
				p = termBox('predicate', id).create().position(top, left);
				predicateBox = $('#' + p.getTid())
					.append("<span class='form'>" + text + "</span>")
					.attr("dataset-triplevalue", text);
				o = termBox('object', id).create().position(top, left + width + boxMargin);
				objectBox = $('#' + o.getTid());

				predicateBox.fadeIn(1000, function(){
					// Add endpoints to the boxes.
					p.addEndpoints();
					if (sid == null) {
						subjectBox.fadeIn(1000);
						s.addEndpoints();
						s.addForm();
					}
					objectBox.fadeIn(1000);
					o.addEndpoints();

					// Add the connections between the predicate and the nodes.
					sConnection = jsPlumb.connect({ source:s.getTid(), target:p.getTid(), sourceEndpoint:s.getEndpoints().right, targetEndpoint:p.getEndpoints().left});
					sConnection.canvas.style.display = 'none';
					oConnection = jsPlumb.connect({ source:o.getTid(), target:p.getTid(), sourceEndpoint:o.getEndpoints().left, targetEndpoint:p.getEndpoints().right});
					oConnection.canvas.style.display = 'none';

					$(sConnection.canvas).fadeIn(500);
					$(oConnection.canvas).fadeIn(500);
					// Add the forms. We don't do this at first because the extra height
					// would make the endpoints center weirdly.
					o.addForm();
				});
			}
		}
	}
  /**
	* Class: termBox
	* The termBox is instantiated for each subject, object, and predicate box.
	* It controls the placement of the box in the canvas, attaching forms and
	* making them function, and attaching endpoints.
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
				endpoints[this.getTid()] = this.endpoints;
			},

			getEndpoints : function(endpoint) {
				return this.endpoints;
			},

			create : function() {
				var box = $("<div class='box'></div>");
				var detachButton = $("<span class='detach'>X</span>");
				var minimizeButton = $("<span class='minimize'>&ndash;</span>");
				var headerBar = $("<div class='header-bar'></div>")
					.append(detachButton)
					.append(minimizeButton);
				detachButton.click(function() {
					var id = $(this).parent().attr("rel");
					jsPlumb.detachAll(id);
					jsPlumb.removeAllEndpoints(id);
					delete endpoints[id];
					$(this).parent().parent().remove();
				});

				var nodeTypeSwitcher = "<div class='form'><input type='checkbox' class='variable' name='" + termPosition + "_type_" + id + "' value='variable'/>"
					+ "<label for='" + termPosition + "_type_" + id + "'>Get as variable</label>&nbsp;"
					+ "<input type='checkbox' class='value' name='" + termPosition + "_type_" + id + "' value='value'/>"
					+ "<label for='" + termPosition + "_type_" + id + "'>Get by value</label></div>";

				if (termPosition == 'predicate') {
					box
						.prepend(headerBar.attr("rel", this.getTid()))
						.addClass("predicate")
						.attr("id", this.getTid())
						.hide();
				}
				else {
					box
						.html(nodeTypeSwitcher)
						.prepend(headerBar.attr("rel", this.getTid()))
						.addClass("rdf-node")
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
					form.attr("class", "form");

					return form;
				};

				var sid = this.getTid();
				var addPredicateButton = $("<div class='add-predicate'><span class='button' rel='" + sid + "'>+</span></div>");
				var predicateList = $("<div id='predicate-list_"+ sid +"' class='list'></div>").attr('rel', sid);
				addPredicateButton.click(function(event) {
					$.ajax({
						type: 'GET',
						url: "/sparql-views/get-predicates",
						dataType: 'html',
						success: function(html, textStatus) {
							$('#predicate-list_'+ sid).append(html);
							$('.predicate').click(function(event) {
								sid = $(this).parent().attr("rel");
								attachedPreds = getPredicates(sid);
								var newPosition = $('#' + attachedPreds[0]).position();
								for (i in attachedPreds) {
									position = $('#' + attachedPreds[i]).position();
									if (position.top > newPosition.top) {
										newPosition.top = position.top;
									}
									newPosition.left = position.left;
								}
								newPosition.top += sparqlViews.boxMarginY + $('#' + sid).height();
								sparqlViews.addTriple('xx', newPosition, sid);
								$(this).parent().slideUp();
							});
						},
						error: function(xhr, textStatus, errorThrown) {
							alert('An error occurred ' + (errorThrown ? errorThrown : xhr.status));
						}
					});
					predicateList.appendTo($(addPredicateButton))
						.fadeIn(2000);
				});

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

				// Attach the elements to the box.
				nodeForm('variable').hide().appendTo($('#' + this.getTid()));
				nodeForm('value').hide().appendTo($('#' + this.getTid()));
				activateSwitcher('variable');
				activateSwitcher('value');
				addPredicateButton.appendTo($('#' + this.getTid()));
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
				jsPlumb.Defaults.Container = $('#workspace');

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

		/*
	 * Class: queryProcessor
	 * This is a singleton that gets instantiated whenever the Process SPARQL
	 * button is clicked. It takes the current state of the visual query builder
	 * and parses SPARQL from that state.
	 */
	var queryProcessor = function () {
		setTriples();
		setPrefixes();
		setPrefixDeclaration();
		setSelectClause();

		function setTriples() {
			var triples = new Object();
			// Add the subject.
			// Get all the subjectEndpoints on this subject.
			var connections = jsPlumb.getConnections();
			this.triples = new Object();

			// Get all of the subject values and store the target predicates as an array.
			for (i in connections['subjectEndpoint']) {
				var connection = connections['subjectEndpoint'][i];
				var subjectLocation = "sourceId";
				var predicateLocation = "targetId";
				addTriple(connection, subjectLocation, predicateLocation);
			}
			for (i in connections['predicateSubjectEndpoint']) {
				var connection = connections['predicateSubjectEndpoint'][i];
				var subjectLocation = "targetId";
				var predicateLocation = "sourceId";
				addTriple(connection, subjectLocation, predicateLocation);
			}
		}

		function addTriple(connection, subjectLocation, predicateLocation) {
			var subjectId = connection[subjectLocation];
			var predicateId = connection[predicateLocation];
			subject = _getNodeValue(subjectId);
			predicate = $('#' + predicateId).attr("dataset-triplevalue");
			object = _getObject(connection, predicateId);

			// If there is a full triple, add this triple to the subject's array.
			if (object) {
			  if (this.triples[subject] == undefined) {
			    this.triples[subject] = new Array();
			  }
			  this.triples[subject].push(new Array(predicate, object));
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

		function setSelectClause() {
			var sparqlQuery = "SELECT * WHERE {\n";
			for (tripleSubject in this.triples) {
				if (this.triples[tripleSubject].length == 1) {
					triple = tripleSubject + " " + this.triples[tripleSubject][0].join(" ");
				}
				else if (this.triples[tripleSubject].length > 1) {
					triple = tripleSubject + " ";
					for (i = 0; i < this.triples[tripleSubject].length; i++) {
						triple += this.triples[tripleSubject][i].join(" ");
						if (i+1 == this.triples[tripleSubject].length) {
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
			this.selectClause = sparqlQuery;
		}

		function setPrefixes() {
			var prefixes = new Array();
			var getPrefix = function(term) {
				httpRegex = /(ftp|http|https):\/\/*/;
				if (!httpRegex.test(term)) {
					splitTerm = term.split(':');
					if (splitTerm[1]) {
						return splitTerm[0];
					}
				}
				return null;
			};
			for (subject in this.triples) {
				prefixes.push(getPrefix(subject));
				for (i in subject) {
					if (this.triples[subject][i]) {
						prefixes.push(getPrefix(this.triples[subject][i][0]), getPrefix(this.triples[subject][i][1]));
					}
				}
			}
			this.prefixes = prefixes;
		}

		function setPrefixDeclaration() {
			var that = this;
			$.ajax({
				type: 'POST',
				async: false,
				url: Drupal.settings.basePath + "sparql-views/prefix-declaration",
				dataType: 'html',
				data: { prefixes: this.prefixes },
				success: function (html, textStatus) {
					that.prefixDeclaration = html;
				},
				error: function (xhr, textStatus, errorThrown) {

				}
			});
		}

		function _getQuery() {
			return this.prefixDeclaration + " " +  this.selectClause;
		}

		function _getSelectClause() {
			return this.selectClause;
		}

		function _getPrefixDeclaration() {
			return this.prefixDeclaration;
		}

		return {
			getPreviewQuery : function() {
				$query = _getQuery();
				return $query + " LIMIT 5";
			},

			getSelectClause : function() {
				return _getSelectClause();
			},

			getPrefixDeclaration : function() {
				return _getPrefixDeclaration();
			}
		}
	}
	function getPredicates(sid) {
		var options = [{'source': sid}];
		var predicates = new Array;
		var connections = jsPlumb.getConnections(options);
		for (i in connections['subjectEndpoint']) {
			predicates.push([connections['subjectEndpoint'][i]['targetId']]);
		}
		delete options.source;
		options.target = sid;
		var connections = jsPlumb.getConnections(options);
		for (i in connections['predicateSubjectEndpoint']) {
			predicates.push([connections['predicateSubjectEndpoint'][i]['sourceId']]);
		}
		return predicates;
	}

  
  /*window.jsPlumb.CurrentLibrary.initDroppable = function(el, options) {
      options['scope'] = options['scope'] || jsPlumb.Defaults.Scope;
      options['accept'] = '._jsPlumb_endpoint';
      el.droppable(options);
   };   */
  /*
   Class: sparqlViews
   
   This is the main entry point to sparqlViews.  Static methods are used instead of methods in jQuery's "$.fn" object itself, to stay in line with jsPlumb, which this depends on.  
   */
    var sparqlViews = window.sparqlViews = {
			init : function() {
				endpoints = new Object();
				this.boxMarginY = 20;
				Drupal.settings.sparql_views.recurssionCount = 0;
			},

      processSparql : function() {
				var qp = queryProcessor();
				var showResultPreview = function() {
					$.ajax({
						type: 'POST',
						url: Drupal.settings.basePath + "sparql-views/get-result-preview",
						dataType: 'html',
						data: {
							endpoint: Drupal.settings.sparql_views.endpoint,
							storeReadKey: Drupal.settings.sparql_views.readKey,
							query: qp.getPreviewQuery()
						},
						success: function(html, textStatus) {
							workspaceWindow = $('#workspace-window');
							previewWindow = $('#preview').html(html);
							previewBottom = previewWindow.height()-workspaceWindow.height();
							$('#preview').hide().css('bottom', previewBottom).show('clip');
							$('body').one('click', function() {
								$('#preview').hide('clip');
								$("#edit-select-clause").hide('clip');
							});
						},
						error: function(xhr, textStatus, errorThrown) {
							alert('An error occurred ' + (errorThrown ? errorThrown : xhr.status));
						}
					});
				}

				showResultPreview();
				// Escape the prefix declaration because otherwise slash URIs get
				// transformed into HTML tags
				// (ie. PREFIX foaf: <http://xmlns.com/foaf/0.1></http>).
				$('#edit-prefixes').text(escape(qp.getPrefixDeclaration()));
				$("#edit-select-clause").html(qp.getSelectClause());
				delete qp;
      },
			
			addBoxes : function(text, position, sid) {
				qb = queryBuilder();
				source = null;
				qb.addBoxes(text, position, sid);
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

  $(document).ready(function() {
		sparqlViews.init();
		$('#predicate-store').hide();

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

		$('#predicate-store-wrapper .loading').hide();
		submitButton = $('button');
		$('#edit-select-clause').hide();
		submitButton.hide();

		$('#predicate-store-wrapper .loading').fadeIn(500);
		getPredicateStore();

		$(".process").click(function() {
			$('#edit-select-clause').show('clip');
      sparqlViews.processSparql();

			$('button').show();
    });

		$('#workspace').draggable();

		// Initialize values from the last form.
		$('#ui-dialog-title-dialog-main').html('SPARQL Query Builder for ' + Drupal.settings.sparql_views.endpoint);
		$('#edit-prefixes').html(Drupal.settings.sparql_views.prefixes).parents('.form-item').hide();

    $("#clear").click(function() { jsPlumb.detachEverything(); });

		$('#predicate-store-toggle-wrapper').click(function () {
			$('#predicate-store').toggle("slow");
		});

		function getPredicateStore() {
			$.ajax({
				type: 'POST',
				url: Drupal.settings.basePath + "get-predicates",
				dataType: 'html',
				data: {
					endpoint: Drupal.settings.sparql_views.endpoint,
					storeReadKey: Drupal.settings.sparql_views.readKey,
					recurssionCount: Drupal.settings.sparql_views.recurssionCount
				},
				success: function(html, textStatus) {
					if (html != 'done') {
						setTimeout(function() { getPredicateStore(); }, 5000);
						$('#predicate-store').append(html).show();
						window.console.log('not done -'  + Drupal.settings.sparql_views.recurssionCount);
						Drupal.settings.sparql_views.recurssionCount += 1;
						$('.predicate').draggable({
							helper: "clone"
						});
					}
					else if (html == 'done') {
						$('#predicate-store-wrapper .loading').hide();
					}
					$('#workspace').droppable({
						accept: '.predicate',
						drop: function(event,ui) {
							// Position the subject, predicate, and object in the workspace.
							sid = null;
							windowPos = $('#workspace-window').offset();
							workspacePos = $('#workspace').position();
							position = {'top': ui.helper.offset().top-workspacePos.top-windowPos.top, 'left': ui.helper.offset().left-workspacePos.left-windowPos.left};
							sparqlViews.addBoxes(ui.draggable.text(), position, sid);
							// Close the predicate store.
							$('#predicate-store').toggle("slow");
						}
					});
					$('input#search').quicksearch('#predicate-store div.predicate');
				},
			  error: function(xhr, textStatus, errorThrown) {
			    alert('An error occurred ' + (errorThrown ? errorThrown : xhr.status));
		    }
	    });
		}
  });
	/* end jQuery */
