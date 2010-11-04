<?php
// $Id$

/**
 * @file sparql_views_ui.tpl.php
 * Termporary theme implementation to display the SPARQL Views visual query
 * builder.
 */
?>
<?php $path = drupal_get_path('module', 'sparql_views');
global $base_path;?>
	<div id="predicate-store-wrapper">
    <h2>Attributes &amp; Relationships</h2>
    <div class="loading">
      <p>Loading predicates from the dataset... this may take a while</p>
      <img src="<?php print $base_path . '/' . $path ?>/images/ajax-loader.gif" />
    </div>
    <div id="predicate-store">
      <form><input type="text" id="search" size="30"></form>
    </div>
    <div id="predicate-store-toggle-wrapper">
      <img id="predicate-store-toggle" class="closed" src="<?php print $base_path . '/' . $path ?>/images/predicate-store-toggle.png" />
    </div>
  </div>
	<div id="workspace-window"><div id="preview"></div><div id="workspace" class="ui-draggable"></div>
</div>
  <span class="process ui-state-default ui-corner-all">Preview Result</span>
<?php
// For the time being, JS is added in the template. When SV is moved to Drupal 7
// the JS placement will be handled properly. Unfortunately, in D6, jQuery UI 1.7
// must be used for Dialog API and 1.8 must be used for jsPlumb. I have no idea
// why doing it this way works, but it does.
 ?>
    <script type="text/javascript" src="http://explorercanvas.googlecode.com/svn/trunk/excanvas.js"></script>
    <script type="text/javascript" src="http://ajax.googleapis.com/ajax/libs/jquery/1.4.2/jquery.min.js"></script>
    <script type="text/javascript" src="http://ajax.googleapis.com/ajax/libs/jqueryui/1.8.0/jquery-ui.min.js"></script>
    <script type="text/javascript" src="<?php print "$base_path$path/js/"?>jquery.jsPlumb-1.2.3-all-min.js"></script>
    <script type="text/javascript" src="<?php print "$base_path$path/js/"?>jquery.quicksearch.js"></script>
    <script type="text/javascript" src="<?php print "$base_path$path/js/"?>sparql_views_ui.js"></script>
