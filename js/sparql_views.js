// $Id: 
(function ($) {

  Drupal.behaviors.prefixSelection = function (context) {
		endpointField = $('#edit-query-options-endpoint-url');
                datasetField = $('#edit-query-options-dataset-url');
		readKeyField = $('#edit-query-options-endpoint-read-key');
		prefixesField = $('#edit-query-options-prefixes');
		selectClauseField = $('#edit-query-options-select-clause');
		Drupal.settings.sparql_views.endpoint = endpointField.val();
                Drupal.settings.sparql_views.dataset = datasetField.val();
		Drupal.settings.sparql_views.readKey = readKeyField.val();
		Drupal.settings.sparql_views.prefixes = prefixesField.val();
		Drupal.settings.sparql_views.selectClause = selectClauseField.val();

		endpointField.blur(function () {
			Drupal.settings.sparql_views.endpoint = $(this).val();
		});
                datasetField.blur(function () {
			Drupal.settings.sparql_views.dataset = $(this).val();
		});
		readKeyField.blur(function () {
			Drupal.settings.sparql_views.readKey = $(this).val();
		});
		prefixesField.blur(function () {
			Drupal.settings.sparql_views.prefixes = $(this).val();
		});
		selectClauseField.blur(function () {
			Drupal.settings.sparql_views.selectClause = $(this).val();
		});
  };
})(jQuery);
