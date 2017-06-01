Smithsonian Exhibition Piece module
===================================

This module provides functionality for a public facing exhibition site. It takes control of the object view and displays the Smithsonian collection in a user friendly way. An easy navigation which shows the parents and children objects of the viewed object should invite the user to browse and discover the collection.

Dependencies:
-------------
- fedora_repository
- sidora_settings
- shadowbox: http://drupal.org/project/shadowbox
- FlexPaper: Download the latest flash version of Flexpaper here http://flexpaper.devaldi.com/download/ and put it in the library folder: /sites/all/libraries/

Credits:
--------
Icons: http://p.yusukekamiyamane.com/


Todo's
------

Done - @TODO: People objects need description added by creating a new Settings class for them (check w Paul) --> DC
Done - @TODO: add sample data to theme fieldbooks and images (Cameratrap and others? check w Paul)
Done - @TODO: Make display tabs to limit resource objects by type (image/fieldbook/etc) (need to know more - will this be dependant on Content model of the parent concept? check w Paul)
Done - @TODO: add concept icon per concept type
Done - @TODO: Clean up the code. Divide everything in separate functions, especially to render the metadata, because that's repeated
Done - @TODO: shadowbox: auto fit html table
Done - @TODO: check if there's a way to check if Flexpaper is put in the library folder correctly
Done - @TODO: I use $content_model->pid in 3 functions. Should limit it to one function call
Done - @TODO: csv popup width + code cleanup
Done - @TODO: Add Flowplayer check also in admin interface.

@TODO: Add flowplayer check on object pages too? (not for now)
@TODO: What if csv files have no header or have another structure than comma separated? Provide fix.
@TODO: Introduce different ways to sort instead of using the table header title
@TODO: Figure out how to deal with Child concept list that are too long (ajax pager, just css scrollbar and/or autocomplete field)
@TODO: Make layout as generic as possible
@TODO: Create a theme
@TODO: check if there's an alternative for hardcoding the library url (seems like there isn't)
@TODO: get shadowbox 'galleries' going again.
@TODO: error on fieldbooks/fedora/repository/si:863 (faulty object - shouldn't be there on production server)