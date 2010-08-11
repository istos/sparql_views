// $Id: 
(function ($) {

  Drupal.behaviors.prefixSelection = function (context) {
		endpointField = $('#edit-query-options-endpoint-url');
		Drupal.settings.sparql_views.endpoint = endpointField.val();

		endpointField.blur(function () {
			Drupal.settings.sparql_views.endpoint = $(this).val();
		});
  };
})(jQuery);
