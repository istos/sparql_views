core = 6.x

projects[views][version] = 3.0-alpha3

; UI make
projects[] = dialog
projects[] = ctools
projects[jquery_update][version] = 2.x-alpha1

projects[] = jquery_ui
libraries[jquery_ui][download][type] = "get"
libraries[jquery_ui][download][url] = "http://jquery-ui.googlecode.com/files/jquery-ui-1.7.3.zip"
libraries[jquery_ui][destination] = "modules/contrib/jquery_ui"
libraries[jquery_ui][directory_name] = "jquery.ui"

; quicksketch