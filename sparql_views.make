core = 6.x

projects[views][version] = 3.0-alpha3
projects[views][patch][] = "http://drupal.org/files/issues/views_621142-16_2_0_1.patch"
projects[views][patch][] = "http://srvgal12.deri.ie/views_621142-39.patch"
libraries[sparql_views][download][type] = "get"
libraries[sparql_views][download][url] = "http://code.semsol.org/source/arc.tar.gz"
libraries[sparql_views][destination] = "modules/sparql_views/lib/rdfx"
libraries[sparql_views][directory_name] = "vendor/arc"

; UI make
projects[] = dialog
projects[] = ctools
projects[jquery_update][version] = 2.0-alpha1

projects[] = jquery_ui
libraries[jquery_ui][download][type] = "get"
libraries[jquery_ui][download][url] = "http://jquery-ui.googlecode.com/files/jquery-ui-1.7.3.zip"
libraries[jquery_ui][destination] = "modules/jquery_ui"
libraries[jquery_ui][directory_name] = "jquery.ui"

; quicksketch