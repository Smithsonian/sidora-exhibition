<?php


/* page preprocess function */
function smithsonian_preprocess_page(&$variables) {
    $variables['mission'] = filter_xss_admin(theme_get_setting('mission'));
//    dsm($variables);
}