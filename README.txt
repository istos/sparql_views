DRUPAL 6 vs. DRUPAL 7
=====================
Please note, while this is being developed in Drupal 6, I do not plan to
maintain a Drupal 6 version of this module.

INSTALL
=======
These are temp installation instructions while SPARQL Views is in heavy
development.

1. Download Views 3 dev
2. Apply the patch in issue http://drupal.org/node/621142.
3. Download the ARC library from http://code.semsol.org/source/arc.tar.gz
   and place it in sparql_views/lib/rdfx/vendor/

HACKS
=====
Because there are some differences between the way RDF datasets and SPARQL work
as opposed to relational databases and SQL, there are some hacks that are
required. If you can think of improvements, please post an issue in the issue
queue at http://drupal.org/project/issues/sparql_views.

1. Defining fields
Because there is no set schema when using SPARQL against arbitrary RDF datasets,
the module cannot define the fields in the sparql_ep base table. Thus, we cannot
define the fields in hook_views_data as we usually would.

Instead, we add the field definition dynamically by looking at the URL to get
the view name and display name and then retriving the variables defined in the
WHERE clauses via the filter system.

2. Handling rows with multi-value fields
In Views, if a query would result in a mutiple values for one field, you simply
perform a secondary query. For instance, "the user roles field actually performs
a secondary query to collect all of the roles for the users listed and put them
together as a single field," (taken from rough draft of Drupal Building Blocks
by Earl Miles et al.)

However, when using SPARQL against arbitrary datasets, we do not know which
variables will return multiple values and running a separate query for each
would not be performant, so we process the results after the query returns.

DRUPAL 6 HACKS
==============
These hacks are necessary because of the differences between Drupal 6 and 7.

1. rdfx_ functions are included in SPARQL Views because they differ from the rdf
functions in 6.

2. The ARC library needs to be placed in SPARQL Views folder, but in D7 the
library is in rdfx.