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
  $variables['limit_types'] = array('all' => t('All'), 'images' => t('Images'), 'pdf' => t('PDF'), 'text' => t('Text'), 'csv' => t('CSV'), 'video' => t('Video'));
  // Results per Page
  $variables['results_per_page'] =  array('10', '15', '20', '25', '30');
  // Table
  $resources = si_exhibition_get_child_resource_info($variables['object'], $show, $sort); // For now we ignore the $limit and offset $offset, we must check each resource.
  si_exhibition_fill_out_resources($resources, $show, $limit, $offset);
  $variables['table'] = theme('si_exhibition_resource_table', array('resources'=>$resources, 'show'=>$show, 'limit'=>$limit, 'offset'=>$offset));
  // Pager
  $variables['total'] = $total = count($resources);
  si_exhibition_pager_init($total, $limit, 0);
  $variables['pager'] = theme('pager', array('BBB_UNKNOWN'=>NULL, 'limit'=>$limit, 'start'=>0, 'BBB_UNKNOWN_2'=>NULL, 'per_page'=>10));
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
  $display_definitions = array(
    'si:imageCModel' => array(
      'dsid' => 'IMAGE',
      'view' => 'IMAGE',
      'url' => 'IMAGE',
      'tn' => TRUE,
      'func' => 'si_exhibition_ds',
      'type' => array('r-jpg', 'jpg'), // Icon, Label
      'class' => 'resources-jpg'
    ),
    'si:generalImageCModel' => array(
      'dsid' => 'IMAGE',
      'view' => 'IMAGE',
      'url' => 'IMAGE',
      'tn' => TRUE,
      'func' => 'si_exhibition_ds',
      'type' => array('r-jpg', 'jpg'), // Icon, Label
      'class' => 'resources-jpg'
    ),
    'si:datasetCModel' => array(
      'dsid' => array('OBJ','CSV'),
      'view' => array('OBJ','CSV'),
      'func' => 'si_exhibition_ds',
      'type' => array('r-csv', 'csv'), // Icon, Label
      'class' => 'resources-csv'
    ),
    'si:fieldbookCModel' => array(
      'dsid' => 'OBJ',
      'view' => 'OBJ.swf',
      'tn' => TRUE,
      'func' => 'si_exhibition_flexpaper',
      'type' => array('r-pdf', 'pdf'), // Icon, Label
      'class' => 'resources-pdf'
    )
  );
  /*
  print '<div style="padding:20px;">';
  var_dump(array_flip($resource['models']));
  print '</div>';
  var_dump(array_intersect_key($display_definitions, array_flip($resource['models'])));
  */
  $display = array_pop(array_intersect_key($display_definitions, array_flip($resource['models']))); // Assuming an object can't be two types of resources.
  if (empty($display)) return; //BBB
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

  $available_view = '';
  if (is_array($display['view'])) foreach($display['view'] as $curr_view) if (array_search($curr_view, $resource['datastreams']) !== FALSE) $available_view = $curr_view;
  if (!empty($available_view)){ $display['view'] = $available_view;  }

  //print '<div>'; var_dump($display); print '</div>';
  if (array_search($display['view'], $resource['datastreams']) !== FALSE) {
    $thumbnail_url = url("fedora/repository/{$resource['pid']}/TN");
    $view_preview = (isset($display['tn']) && array_search('TN', $resource['datastreams']) !== FALSE) ? "<img class='resource-tn' src='$thumbnail_url'/>" : '';
    if (isset($display['url'])) {
      //if (empty($view_preview)) $view_preview = t('View Metadata');
      $view = '<a href="' . url("fedora/repository/{$resource['pid']}/{$display['url']}") . '" rel="shadowbox[' . $resource['pid'] . ']" title="' . $resource['label'] . '">' . $view_preview . '</a>';
    }
    else {
      $display_datastream_function = "return {$display['func']}('{$resource['pid']}', '{$display['view']}');";
      if (empty($view_preview))
        $view = '<a href="#" rel="shadowbox[' . $resource['pid'] . ']" title="' . $resource['label'] . '" onclick="' . $display_datastream_function . '">Preview</a>';
      else
        $view = '<a href="#" rel="shadowbox[' . $resource['pid'] . ']" title="' . $resource['label'] . '" onclick="' . $display_datastream_function . '">' . $view_preview . '</a>';
    }
  }
  // Type
  list($icon, $type) = $display['type'];
  $type = "<div class='r-icon $icon title='$type'>$type</div>";
  // Download
  $download = '';

  $available_dsid = '';
  if (is_array($display['dsid'])) foreach($display['dsid'] as $curr_dsid) if (array_search($curr_dsid, $resource['datastreams']) !== FALSE) $available_dsid = $curr_dsid;
  if (!empty($available_dsid)){ $display['dsid'] = $available_dsid;  }

  if (array_search($display['dsid'], $resource['datastreams']) !== FALSE) {
    $download = l('Download', 'fedora/repository/' . $resource['pid'] . '/' . $display['dsid'], array('class'=> $display['class']));
  }
  $option_dropdown = '<div class="exhibition_js_dropdown" style="display:none;"><select class="exhibition_option_dd" id="exhibition_option_dd_'.substr($resource['pid'],3).'"><option value="choose">Choose...</option><option value="preview">Preview</option><option value="meta">View Metadata</option><option value="download">Download</option></select></div>';
  $option_dropdown .= '<div class="regular-options">'.$download.$metadata;
  $option_dropdown .= '<a href="#" rel="shadowbox[' . $resource['pid'] . ']" title="' . $resource['label'] . '" onclick="' . $display_datastream_function . '">Preview</a>';
  $option_dropdown .= '</div>';
  $option_dropdown .= "\n".'<script>(function($){';
  $jquery_item = '$("#exhibition_option_dd_'.substr($resource['pid'],3).'")';
  $option_dropdown .= $jquery_item.'.change(function(){';
  $option_dropdown .= "\n".'var val = '.$jquery_item.'.val();';
  $option_dropdown .= "\n".'if (val  == "meta"){';
  $option_dropdown .= $display_metadata_function;
  $option_dropdown .= "\n".'}else if (val == "preview"){';
  $option_dropdown .= "
 Shadowbox.open({
content: '".url("fedora/repository/{$resource['pid']}/{$display['url']}")."',
player: 'iframe',
player: 'iframe',
title: '".$resource['label'] ."',
}); ";
  //$option_dropdown .= $display_datastream_function;
  $option_dropdown .= '}else if (val == "download"){';
  $option_dropdown .= ' window.open("'.url("fedora/repository/{$resource['pid']}/{$display['url']}").'","Download");';
  $option_dropdown .= '}';
  $option_dropdown .= '});';
  $option_dropdown .= '})(jQuery);</script>';
  return array($view, '<span class="table-label">'.$label.'<span>', $option_dropdown); //$metadata, $type, $download);
}

/**
 * Implements theme_hook().
 *
 * Displays both the parent/child concept links.
 */
function theme_si_exhibition_navigation($vars){
  $object = $vars['object'];
  $output = '<div id="si-navigation"><div id="si-navigation-inner">';
  $output .= (variable_get('si_exhibition_expose_block', 'no') == 'no' ? '<h2>' . t('Navigation') . '</h2>' : '');
  $output .= theme('si_exhibition_parent_concepts', array('object'=>$object));
  $output .= theme('si_exhibition_child_concepts',  array('object'=>$object));
  $output .= '</div></div>';
  return $output;
}

/**
 * Implements theme_hook().
 *
 * Displays a list of parent concepts links.
 */
function theme_si_exhibition_parent_concepts($vars) {
  $object = $vars['object'];
  global $main_path;
  $main_path = (isset($_GET['path']) ? $_GET['path'] : '');
  if (!empty($main_path)) {
    $lastComma = strrpos($main_path, ",");
    if ($lastComma === false) {
      $main_path = '';
    } else {
      $main_path = substr($main_path, 0, $lastComma);
    }
  }
  
  $header = '<h3>' . t('Parent concepts') . '</h3>';
  $links = array_map(
    function($o) { 
      global $main_path;
      return l($o['label'], "fedora/repository/{$o['pid']}", array('query' => array('path'=>$main_path))); 
    },
    si_exhibition_get_parent_info($object, TRUE)
  );
  $item_list_array = array(
    'items'=>$links,
    'title'=>NULL,
    'type'=>'ul',
    'attributes'=>array('class' => 'parent-concepts'),
  );
  return $header . (empty($links) ? t('This item has no parent concepts.') : theme('item_list', $item_list_array));
}

/**
 * Implements theme_hook().
 *
 * Displays a list of child concepts links along with their number of children.
 */
function theme_si_exhibition_child_concepts($vars){
    $object = $vars['object'];
    //FedoraObject $object) {
  global $main_path;
  $header ='<h3>' . t('Child concepts') . '</h3>';
  $main_path = (isset($_GET['path']) ? $_GET['path'] : '');
  if (empty($main_path)){
    $main_path = $object->id;
  }else{
    $main_path .= ','.$object->id;
  }
  $make_link = function($o) {
    global $main_path;
    return l($o['label'] . (($o['count'] > 0) ? " ({$o['count']})" : ''), "fedora/repository/{$o['pid']}", array('query' => array('path'=>$main_path))); 
  };
  
  $links = array_map($make_link,  si_exhibition_get_child_concept_info($object));
  //var_dump($links);
  $item_list_array = array(
    'items'=>$links, 
    'title'=>NULL, 
    'type'=>'ul', 
    'attributes'=>array('class' => 'child-concepts'),
  );
  return $header . (empty($links) ? t('This item has no child concepts.') : theme('item_list', $item_list_array));
}
/*
 * Returns the xml transformed by xsl, determined by the workbench if available, and the default xsl if not
 *
 */
function theme_si_exhibition_metadata_xmls($vars){
  $object = $vars['object'];
  $rows = si_models_get_display_xmls($object->id);
  $datastream_content =  si_exhibition_meta_string_for_xslt($object);
  $output = $datastream_content;
  $doc = new DOMDocument();
  $xsl = new XSLTProcessor();
  $xsl_string = si_exhibition_xsl_string($object->id);
  if ($xsl_string === FALSE){
    //No XSL string (bad news)
    drupal_set_message("Configuration problem, could not obtain XSL information from configured XSL provider. This usually means the exhibition server cannot contact the workbench server.", "error");
    return '';
  }
  $doc->loadXML(si_exhibition_xsl_string($object->id));
  $xsl->importStyleSheet($doc);
  $docToLoad = ('<?xml version="1.0" encoding="UTF-8"?'.'>'.$output);
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
    $object = $vars['object'];
    //FedoraObject $object) {
  $rows = si_models_get_display_fields($object->id);
 //echo theme('table', $rows);
  return ($rows > 0) ? theme('table', array(), $rows, array('class' => 'metadata-table')) : t('This item has no metadata.');
  //return ($rows > 0) ? theme('table', array(), $rows, array('class' => 'metadata-table')) : t('This item has no metadata.');
}

/**
 * Implements theme_hook().
 *
 * Renders An html repersentation of the TEXT datastream.
 */
function theme_si_exhibition_text_datastream($vars){
    $object = $vars['object'];
    //FedoraObject $object) {
  return nl2br(si_exhibition_get_text_datastream_content($object), TRUE);
}


/**
 * Implements theme_hook().
 *
 * An html repersentation of the CSV datastream.
 */
function theme_si_exhibition_csv_datastream($vars){
    $object = $vars['object'];
    //FedoraObject $object) {
  $filepath = 'public://csv.txt';
  $content = si_exhibition_get_csv_datastream_content($object);
  file_save_data($content, $filepath, FILE_EXISTS_REPLACE);
  $rows = si_exhibition_transform_csv_file_to_array($filepath);
  unlink($filepath);
  $map_row = function($o) { return array('data' => $o); };
  $rows = array_map(function($o) use($map_row) { return array_map($map_row, $o); }, $rows);
  $header = array_shift($rows);
  return theme('table', array('header' => $header, 'rows' => $rows));
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
  $object = $vars['object'];
  $models = implode(' ', $object->models);
  $classes = strtolower(preg_replace('/[^A-Za-z0-9 ]/', '-', $models) ); // Make valid css class
  $title = check_plain($object->label);
  //return "<span class='title'>$title</span><span class='si-icon $classes'></span>";
  return "$title";//<span class='si-icon $classes'></span>";
}
