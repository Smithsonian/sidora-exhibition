<?php
/**
 * @file
 *
 * This modules theme/theme_preprocess functions.
 */
module_load_include('inc', 'si_exhibition', 'si_exhibition.util');

/**
 * Implements hook_preprocess_theme().
 *
 * Renders the object page.
 */
function si_exhibition_preprocess_si_exhibition_object(array &$vars) {
  $object = $vars['object'];
  $models = implode(' ', $object->models);
  $vars['models'] = $object->models;
  $vars['model_classes'] = strtolower(preg_replace('/[^A-Za-z0-9 ]/', '-', $models) ); // Make valid css classes
  $vars['block_exposed'] = $block_exposed = variable_get('si_exhibition_expose_block', 'no');
  $vars['layout'] = ($block_exposed == 'no' ? '' : 'si-exhibition-full-width');
}

/**
 * Implements hook_preprocess_theme().
 *
 * Renders a table of Resources.
 */
function si_exhibition_preprocess_si_exhibition_resources(array &$variables) {
  list($show, $sort, $limit, $page) = $pager_params = si_exhibition_get_pager_parameters();
  $pager_params = array_combine(array('show', 'sort', 'limit', 'page'), $pager_params);
  $variables = array_merge($variables, $pager_params, array('offset' => $offset = ($limit * $page)));
  // Create a link for the given paging param.
  $pager_link = function($text, $param, $value) use ($pager_params) {
    $current = ($pager_params[$param] == $value);
    $pager_params[$param] = $value;
    unset($pager_params['page']); // When changing type or results per page return to the first page
    return $current ? '<strong>' . $text . '</strong>' : l($text, $_GET['q'], array('query' => $pager_params));
  };
  // Limit types
  $variables['limit_types'] = array('all' => t('All'), 'images' => t('Images'), 'pdf' => t('PDF'), 'text' => t('Text'), 'csv' => t('CSV'), 'audio' => t('Audio'), 'video' => t('Video'), 'generic' => t('Generic'), 'rawsequence' => t('Raw Sequences'), 'assembledsequence' => t('Assembled Sequences'));
  // Results per Page
  $variables['results_per_page'] =  array('10', '15', '20', '25', '30');
  // Table
  $resources = si_exhibition_get_child_resource_info($variables['object'], $show, $sort); // For now we ignore the $limit and offset $offset, we must check each resource.
  si_exhibition_fill_out_resources($resources, $show, $limit, $offset);
  $variables['table'] = theme('si_exhibition_resource_table', array('resources'=>$resources, 'show'=>$show, 'limit'=>$limit, 'offset'=>$offset));
  // Pager
  $variables['total'] = $total = count($resources);
  si_exhibition_pager_init($total, $limit, 0);
  $variables['pager'] = theme('pager', array('limit'=>$limit, 'start'=>0, 'per_page'=>10));
}

/**
 * Implements theme_hook().
 *
 * Displays the table of resources.
 */
function theme_si_exhibition_resource_table($vars){
  //array $resources, $show, $offset, $limit) {
  $resources = $vars['resources'];
  $show = $vars['show'];
  $offset = $vars['offset'];
  $limit = $vars['limit'];
  if (!is_array($resources)) {
    return;  //No resources then nothing to do
  }
  $rows = array_map(function($o) { return _si_exhibition_get_resource_table_row($o); }, $resources);
  $rows = array_slice($rows, $offset, $limit);
  if (!empty($rows)) {
    $header = array(
      array('data' => ''),
      array('data' => t('Title'), 'sort' => 'asc'), 
      array('data' => t('Options')),
    );
    return theme('table', array('header'=>$header, 'rows'=>$rows));
  }
  else {
    //$message = ($show == 'all') ? t('This item has no resources.') : t('No resources of type "@type" found.', array('@type' => $limit_types[$show]));
    $message = ($show == 'all') ? t('This item has no resources.') : t('No resources of the selected type found.');
    return "<div class='no-results'>$message</div>";
  }
}

/**
 * Fetchs a row definition for the given resource.
 *
 * @param array $resource
 *   An array describing a particular resource.
 *
 * @return array
 *   A drupal table row definition for using in the resources table.
 */
function _si_exhibition_get_resource_table_row(array $resource) {
  if (empty($resource['models'])) {return '';}
  $display_datastream_function = '';
  // @todo use drupal behaviours and settings instead.
  $display = array();
	$display_definitions = array(
    'si:imageCModel' => array(
      'dsid' => 'IMAGE',
      'view' => 'IMAGE',
      'url' => 'sidora_viewer',
      'tn' => TRUE,
      'func' => 'si_exhibition_ds',
      'type' => array('r-jpg', 'jpg'), // Icon, Label
      'class' => 'resources-jpg'
    ),
    'si:generalImageCModel' => array(
      'dsid' => 'OBJ',
      'view' => 'OBJ',
      'url' => 'sidora_viewer',
      'tn' => TRUE,
      'func' => 'si_exhibition_ds',
      'type' => array('r-jpg', 'jpg'), // Icon, Label
      'class' => 'resources-jpg'
    ),
    'si:datasetCModel' => array(
      'dsid' => array('OBJ','CSV'),
      'view' => array('OBJ','CSV'),
      'url' => 'sidora_viewer',
      'tn' => TRUE,
			'func' => 'si_exhibition_ds',
      'type' => array('r-csv', 'csv'), // Icon, Label
      'class' => 'resources-csv'
    ),
    'si:fieldbookCModel' => array(
      'dsid' => 'OBJ',
      'view' => 'OBJ',
      'url' => 'sidora_viewer',
      'tn' => TRUE,
      'func' => 'si_exhibition_flexpaper',
      'type' => array('r-pdf', 'pdf'), // Icon, Label
      'class' => 'resources-pdf'
    ),
    'islandora:sp-audioCModel' => array(
      'dsid' => array('OBJ'),
      'view' => array('PROXY_MP3'),
      'url' => 'sidora_viewer',
      'tn' => TRUE,
      'func' => 'si_exhibition_ds',
      'type' => array('r-csv', 'csv'), // Icon, Label
      'class' => 'resources-audio'
    ),
    'islandora:sp_videoCModel' => array(
      'dsid' => array('OBJ'),
      'view' => array('MP4'),
      'tn' => TRUE,
      'url' => 'sidora_viewer',
      'func' => 'si_exhibition_ds',
      'type' => array('r-csv', 'csv'), // Icon, Label
      'class' => 'resources-video'
    ),
    'si:genericCModel' => array(
      'dsid' => '',
      'view' => '',
      'tn' => TRUE,
      'url' => '',
      'func' => 'si_exhibition_ds',
      'type' => array('r-csv', 'csv'), // Icon, Label
      'class' => 'resources-generic'
    ),
    'si:rawGeneSequenceCModel' => array(
      'dsid' => '',
      'view' => '',
      'tn' => TRUE,
      'url' => '',
      'func' => 'si_exhibition_ds',
      'type' => array('r-csv', 'csv'), // Icon, Label
      'class' => 'resources-genome'
    ),
    'si:assembledGeneSequenceCModel' => array(
      'dsid' => '',
      'view' => '',
      'tn' => TRUE,
      'url' => '',
      'func' => 'si_exhibition_ds',
      'type' => array('r-csv', 'csv'), // Icon, Label
      'class' => 'resources-genome'
    ),
  );
  /*
  print '<div style="padding:20px;">';
  var_dump(array_flip($resource['models']));
  print '</div>';
  var_dump(array_intersect_key($display_definitions, array_flip($resource['models'])));
  */
  $aik = array_intersect_key($display_definitions, array_flip($resource['models']));
  $display = array_pop($aik); // Assuming an object can't be two types of resources.
  $display['url'] = (isset($display['url'])?$display['url']:'');
  $label = $resource['label'];
  // Metadata
  //$display_metadata_function = 'return si_exhibition_ds("' . $resource['pid'] . '", "METADATA")';
  $display_metadata_function = 'return si_exhibition_ds("' . $resource['pid'] . '", "METADATA_XMLS")';
  $metadata = "<a href='#' rel='shadowbox' title='{$resource['label']}' onclick='{$display_metadata_function}'>" . t('View Metadata') . '</a>';
  // View
  //Serach through the types we care about to find a datastream that's available
  //Originally set up as a single DSID, shoehorning in having datastream options
  $view = '';
  if ($resource['visibility']['show_preview']){ //Do not show the "preview" thumbnail & link if preview is off
    $available_view = '';
    if (is_array($display['view'])) foreach($display['view'] as $curr_view) if (array_search($curr_view, $resource['datastreams']) !== FALSE) $available_view = $curr_view;
    if (!empty($available_view)){ $display['view'] = $available_view;  }
    $preview = '';
    if (array_search($display['view'], $resource['datastreams']) !== FALSE) {
	//$thumbnail_url = url("fedora/repository/{$resource['pid']}/TN/tn.jpg")."?path=".implode(',',si_exhibition_get_pid_path_to_showing_concept());
	$thumbnail_url = url("si/exhibition/tn/{$resource['pid']}/TN/tn.jpg")."?path=".implode(',',si_exhibition_get_pid_path_to_showing_concept());
      if (array_search('si:datasetCModel', $resource['models']) !== FALSE) {
        $thumbnail_url = $GLOBALS['base_url'].'/'.drupal_get_path('module','si_exhibition').'/images/icons/x-office-spreadsheet.png';
      }	
      if ((array_search('si:fieldbookCModel', $resource['models']) !== FALSE) && (array_search('TN', $resource['datastreams']) === FALSE)){
        $thumbnail_url = $GLOBALS['base_url'].'/'.drupal_get_path('module','si_exhibition').'/images/icons/application-pdf.png';
      }	
     $view_preview = (isset($display['tn']) && (array_search('TN', $resource['datastreams']) !== FALSE || array_search('si:datasetCModel', $resource['models']) !== FALSE || in_array('si:fieldbookCModel',$resource['models']))) ? "<img alt='image thumbnail for {$resource['label']}' class='resource-tn' src='$thumbnail_url'/>" : '';
      if (isset($display['url'])) {
        $pptsc = si_exhibition_get_pid_path_to_showing_concept();
        $path_including_last = implode(',',$pptsc);
	  $prefix = '<a href="' . url("fedora/repository/{$resource['pid']}/{$display['url']}")."?path=".$path_including_last . '" rel="shadowbox" title="' . $resource['label'] . '">';
       $postfix = '</a>';
       $view = $prefix . $view_preview . $postfix;
       $preview = $prefix . 'Preview' . $postfix;
      }
      else {
        $display_datastream_function = "return {$display['func']}('{$resource['pid']}', '{$display['view']}');";
        if (empty($view_preview)) {
          $prefix = '<a href="#" rel="shadowbox[' . $resource['pid'] . ']" title="' . $resource['label'] . '" onclick="' . $display_datastream_function . '">';
          $postfix = '</a>';
          $view = $prefix . 'Preview' . $postfix;
          $preview = $prefix . 'Preview' . $postfix;
        } 
        else {
          $prefix = '<a href="#" rel="shadowbox[' . $resource['pid'] . ']" title="' . $resource['label'] . '" onclick="' . $display_datastream_function . '">';
          $postfix = '</a>';
          $view = $prefix . $view_preview . $postfix;
          $preview = $prefix . 'Preview' . $postfix;
        }
      }
    }
  }

  // Type
  list($icon, $type) = $display['type'];
  $type = "<div class='r-icon $icon title='$type'>$type</div>";
  // Download
  $download = '';

  $available_dsid = '';
  if (is_array($display['dsid'])){
    foreach($display['dsid'] as $curr_dsid) {
      if (array_search($curr_dsid, $resource['datastreams']) !== FALSE) {
        $available_dsid = $curr_dsid;
      }
    }
  }
  if (!empty($available_dsid)){ $display['dsid'] = $available_dsid;  }

  if (array_search($display['dsid'], $resource['datastreams']) !== FALSE) {
		$download = l('Download', 'fedora/download/' . $resource['pid'], array('class'=> $display['class']));
	}

  if (empty($preview)){
    $prefix = '<a href="' . url("fedora/repository/{$resource['pid']}/{$display['url']}") . '" rel="shadowbox" title="' . $resource['label'] . '">';
    $postfix = '</a>';
    $preview = $prefix . 'Preview' . $postfix;
  }
  $option_dropdown = '<div class="regular-options">';
  if ($resource['visibility']['show_meta'])      $option_dropdown .= $metadata;
  if ($resource['visibility']['show_preview'])   $option_dropdown .= $preview;
  if ($resource['visibility']['allow_download']) $option_dropdown .= $download;
  $option_dropdown .= '</div>';
  return array($view, '<span class="table-label">'.$label.'<span>', $option_dropdown); //$metadata, $type, $download);
}

/**
 * Implements theme_hook().
 *
 * Displays both the parent/child concept links.
 */
function theme_si_exhibition_navigation($vars){
  if (array_key_exists('pid',$vars)){
    $pid = $vars['pid'];
  }else if (array_key_exists('object',$vars)){
    $pid = $vars['object']->id;
  } else{
    return '';
  }
  $output = '<div id="si-navigation"><div id="si-navigation-inner">';
  $output .= (variable_get('si_exhibition_expose_block', 'no') == 'no' ? '<h2>' . t('Navigation') . '</h2>' : '');
  $output .= theme('si_exhibition_parent_concepts', array('pid'=>$pid));
  $output .= theme('si_exhibition_child_concepts',  array('pid'=>$pid));
  $output .= '</div></div>';
  return $output;
}

/**
 * Implements theme_hook().
 *
 * Displays a list of parent concepts links.
 */
/*
function theme_si_exhibition_parent_concepts($vars) {
  $pid = $vars['pid'];
  global $main_path;
  $full_path = (isset($_GET['path']) ? $_GET['path'] : '');
  $main_path = $full_path;
	if (!empty($main_path)) {
    $lastComma = strrpos($main_path, ",");
    if ($lastComma === false) {
      $main_path = '';
    } else {
      $main_path = substr($main_path, 0, $lastComma);
    }
  }
  $full_tree = si_exhibition_get_visibility_tree(); 
  $path_pieces = explode(',',$full_path);
  //Go down through the visibility tree until we end up at the parent
  $curr_tree_loc = $full_tree;
  if (empty($curr_tree_loc)) return '';
  foreach($path_pieces as $piece) {
    foreach($curr_tree_loc['children'] as $ctl_child) {
      if ($ctl_child['pid'] == $piece) {
        $curr_tree_loc = $ctl_child;
	break;
      }
    }
  }
  $header = '<h3>' . t('Parent concept') . '</h3>';
  $links = array();
  if (!empty($full_path)) {
    $link = l($curr_tree_loc['label'], "fedora/repository/{$curr_tree_loc['pid']}", array('query' => array('path'=>$main_path)));
    $links = array($link);
  }
  $item_list_array = array(
    'items'=>$links,
    'title'=>NULL,
    'type'=>'ul',
    'attributes'=>array('class' => 'parent-concepts'),
  );
  return $header . (empty($links) ? t('This item is the root of the exhibition.') : theme('item_list', $item_list_array));
}
*/
function theme_si_exhibition_parent_concepts($vars) {
  si_exhibition_debug('theme_si_exhibition_parent_concepts, pid:'.$vars['pid']);
  $pid = $vars['pid'];
  global $main_path;
  $full_path = (isset($_GET['path']) ? $_GET['path'] : '');
  $main_path = $full_path;
  if (!empty($main_path)) {
    $pid_path_array = explode(',',$main_path);
    $pid_array_values = array_values(array_slice($pid_path_array, -1));
    $last_parent = $pid_array_values[0];
		/* changes
		$last_parent = array_pop($pid_path_array);
    array_push($pid_path_array,$last_parent);
		*/
    $path_to_last_parent = implode('_',$pid_path_array);
    $full_tree = si_exhibition_get_visibility_tree($last_parent,$path_to_last_parent); 
    $curr_tree_loc = $full_tree;
    if (empty($curr_tree_loc)) return '';
    $lastComma = strrpos($main_path, ",");
    if ($lastComma === false) {
      $main_path = '';
    } else {
      $main_path = substr($main_path, 0, $lastComma);
    }
  }
  $header = '<h3>' . t('Parent concept') . '</h3>';
  $links = array();
  if (!empty($full_path)) {
    $link = l($curr_tree_loc['label'], "fedora/repository/{$curr_tree_loc['pid']}", array('query' => array('path'=>$main_path)));
    $links = array($link);
  }
  $item_list_array = array(
    'items'=>$links,
    'title'=>NULL,
    'type'=>'ul',
    'attributes'=>array('class' => 'parent-concepts'),
  );
  return $header . (empty($links) ? t('This item is the root of the exhibition.') : theme('item_list', $item_list_array));
}

/**
 * Implements theme_hook().
 *
 * Displays a list of child concepts links along with their number of children.
 */
function theme_si_exhibition_child_concepts($vars){
  si_exhibition_debug('theme_si_exhibition_child_concepts, pid:'.$vars['pid']);
  $pid = $vars['pid'];
  global $main_path;
  $header ='<h3>' . t('Child concepts') . '</h3>';
  $main_path = (isset($_GET['path']) ? $_GET['path'] : '');
  if (empty($main_path)){
    $main_path = $pid;
  }else{
    $main_path .= ','.$pid;
  }
  $make_link = function($child) {
    global $main_path;
    if (strtolower($child['type']) == 'resource') return NULL;
    return l($child['label'], "fedora/repository/{$child['pid']}", array('query' => array('path'=>$main_path)));
  };
  $vo = si_exhibition_get_visibility_object_children($pid);
  if (empty($vo)) return '';
  if (empty($vo['children'])) {
    $vo['children'] = array();
  }
  $links = array_filter(array_map($make_link, $vo['children']));
  
  $item_list_array = array(
    'items'=>$links, 
    'title'=>NULL, 
    'type'=>'ul', 
    'attributes'=>array('class' => 'child-concepts'),
  );
  return $header . (empty($links) ? t('This item has no child concepts.') : theme('item_list', $item_list_array));
}
/*
 * Error handler to eat the annoying XmlErrors that are not exceptions
 * Returning false in function HandleXmlError() causes a fallback to the default error handler.
 * Originally from: http://php.net/manual/en/domdocument.loadxml.php
 */
function HandleXmlError($errno, $errstr, $errfile, $errline)
{
  if ($errno==E_WARNING && (substr_count($errstr,"DOMDocument::loadXML()")>0))
  {
    watchdog("exhibition",'HandleXmlError:'.$errstr);
    throw new DOMException($errstr);
  }
  else 
  {
    return false;
  }
}

/*
 * Returns the xml transformed by xsl, determined by the workbench if available, and the default xsl if not
 *
 */
function theme_si_exhibition_metadata_xmls($vars){
  $pid = $vars['pid'];
  si_exhibition_debug('in theme metadata, pid:'.$pid);
  if (variable_get('si_exhibition_anonymous_only', FALSE)){
    if (!si_exhibition_check_view_permission($pid)){
      si_exhibition_debug('view_permission was false');
      watchdog("exhibition","in theme metadata, view_permission was false:".$pid);
      drupal_not_found();
      return;
    }
  }
  $parent_pid_path = si_exhibition_get_pid_path_to_showing_concept();
  // changes
	$pid_array_values = array_values(array_slice($parent_pid_path, -1));
	if ((!empty($pid_array_values)) && ($pid_array_values[0] == $pid)) array_pop($parent_pid_path);
	//array_push($parent_pid_path, $pid);
	/* If getting the metadata for a concept, it will be the last element in parent pid path.
	 * si_exhibition_get_visibility_object adds the pid tot he parent pid path before sending for visibility request.
	 * If getting the metadata for a resource, we don't need to do anything, since it will not be in the parent pid path
	*/ 
  $vis_obj = si_exhibition_get_visibility_object($pid, $parent_pid_path);
	if (empty($vis_obj) || empty($vis_obj['show_meta'])) {
    //The page may be showing the metadata on a portion of the page and not a page of only metadata
    //If that is the case, just return without indication that metadata is not shown.
    si_exhibition_debug('empty visibility object or show_meta empty:'.$pid);
    watchdog("exhibition",'in theme metadata,empty visibility object or show_meta empty:'.$pid);
    return '';
  }
  $rows = si_models_get_display_xmls($pid);
  $datastream_content =  si_exhibition_meta_string_for_xslt_by_id($pid);
  $output = $datastream_content;
  $doc = new DOMDocument();
  $xsl = new XSLTProcessor();
  $pid_to_check = $pid;
  if (strpos(strtolower($vis_obj['type']),'concept') !== false) {
    $pid_to_check = si_exhibition_get_parent_xsl($pid);
  } 
  $xsl_string = si_exhibition_xsl_string($pid_to_check);
  if ($xsl_string === FALSE){
    //No XSL string (bad news)
    //drupal_set_message("Configuration problem, could not obtain <a href="errorlog.txt"></a>XSL information from configured XSL provider. This usually means the exhibition server cannot contact the workbench server.", "error");
    watchdog("exhibition",'xsl_string was bad for:'.$pid);
    return '';
  }
  try{
    set_error_handler('HandleXmlError');
    $doc->loadXML($xsl_string);
    restore_error_handler();
  }catch(Exception $e){
    dsm($e);
    dsm($xsl_string);
    watchdog("exhibition",'xsl_string could not load for:'.$pid);
    return '';
  }

  $xsl->importStyleSheet($doc);
  $docToLoad = $datastream_content;
  if (strpos($datastream_content, '<?xml version=') === false) {
    $docToLoad = ('<?xml version="1.0" encoding="UTF-8"?'.'>'.$datastream_content);
  }  
  $doc->loadXML($docToLoad);
  if (!empty($_GET['output'])){
    if ($_GET['output'] == 'xslSaved'){
      print $doc->saveXML();
      exit();
    }
    if ($_GET['output'] == 'xml'){
      print $docToLoad;
      exit();
    }
  
    if ($_GET['output'] == 'phpTransform'){
      print $xsl->transformToXML($doc);
      exit();
    }
    if ($_GET['output'] == 'xsl'){
      print si_exhibition_xsl_string();
      exit();
    }
  }
  $initialOutput = $xsl->transformToXML($doc);
  //now go through and add the even/odd classes
  $tableRows = explode("<tr>",$initialOutput);
  $properOutput = $tableRows[0];
  for ($trNum = 1; $trNum < count($tableRows); $trNum++){ //end one doesn't get a tr
    if ($trNum % 2 == 0){
      $properOutput .= '<tr class="even">'.$tableRows[$trNum];
    }else{
      $properOutput .= '<tr class="odd">'.$tableRows[$trNum];
    }
  }

	return $properOutput; 
}
/**
 * Implements theme_hook().
 *
 * Renders the An objects metadata as a Drupal table.
 */
function theme_si_exhibition_metadata($vars){
  $pid = $vars['pid'];
  $rows = si_models_get_display_fields($pid);
  return ($rows > 0) ? theme('table', array(), $rows, array('class' => 'metadata-table')) : t('This item has no metadata.');
}


/**
 * Initialize a pager for theme('pager') without running an SQL query.
 *
 * The nuts and bolts of this function are borrowed from z3950.module
 * and also apachesolr.module.
 *
 * @see pager_query()
 *
 * @param $total
 *  The total number of items found.
 * @param $limit
 *  The number of items you will display per page.
 * @param $element
 *  An optional integer to distinguish between multiple pagers on one page.
 *
 * @return
 *  The current page for $element. 0 by default if $_GET['page'] is empty.
 */
function si_exhibition_pager_init($total, $limit = 10, $element = 0) {
  global $pager_page_array, $pager_total, $pager_total_items;
  $page = isset($_GET['page']) ? $_GET['page'] : '';
  $pager_page_array = explode(',', $page);
  $pager_total_items[$element] = $total;
  $pager_total[$element] = ceil($pager_total_items[$element] / $limit);
  $pager_page_array[$element] = max(0, min((int)$pager_page_array[$element], ((int)$pager_total[$element]) - 1));
  return $pager_page_array[$element];
}

/**
 * Gets the page title for the given object.
 *
 * @param FedoraObject $object
 *   The object in question.
 *
 * @return string
 *   The title if successful, an empty string otherwise.
 */
function theme_si_exhibition_page_title($vars){
  $title = check_plain($vars['label']);
  return "$title";
}
