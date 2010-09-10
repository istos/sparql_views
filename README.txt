$Id$

DRUPAL 6 vs. DRUPAL 7
=====================
Please note, while this is being developed in Drupal 6, I do not plan to
maintain a Drupal 6 version of this module. It will be released in Drupal 7 as
soon as there are stable versions of all of the required modules.

INSTALL
=======
These are temp installation instructions while SPARQL Views is in heavy
development. This requires Drush.

1. In the command line from the site root, run this command:
   drush make --no-core sites/all/modules/sparql_views/sparql_views.make
2. Enable SPARQL Views, RDFx, Views, and Views UI. If you want to use the
   SPARQL Views drag-and-drop interface, enable SPARQL Views UI and it's
   dependencies.
3. Go to the Tools tab in Views and Disable the Views data cache. If you want
   to use SPARQL Views UI, you unfortunately have to turn off Views JavaScript
   currently as well.

CREATING A SPARQL QUERY
=======================
Go to the Settings pane in Views, in left hand column under Advanced Settings.

Queries can be input in two ways:

1. Enter your prefixes and select query directly in the Settings pane. The
   select query should be in the form SELECT * WHERE {?s ?p ?o} LIMIT 100. You
   will want to limit your query if it returns a large result, otherwise you
   might get the white screen of death because of PHP timeout.
2. Use the drag-and-drop interface by clicking on 'Build query'. This is
   currently very limited and can only create conjunctive queries.
   
After entering the query, any variables used will show up as fields. If you do
not see fields, check to ensure your query saved. If it did, ensure that you
have the Views data cache turned off as described in Install step 3.




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

1. rdfx is included in SPARQL Views as a pseudo library. These functions are
part of the contrib RDF module in Drupal 7, this is a hackish backport.

2. The ARC library needs to be placed in SPARQL Views folder, but in D7 the
library is in rdfx.