<?php

/**
 * @file
 *
 * Utility functions that provided simplicity or convenience.
 */

/**
 * Given an object info array as defined by the *_info() functions, determine if the user can access it.
 *
 * @param array $object_info
 *   An array containing a 'pid' field.
 *
 * @return boolean
 *  TRUE if accessible FALSE otherwise.
 */
function si_exhibition_check_object_info_accessible(array $object_info) {
  return TRUE;
  //Until we are different about permissions, assume the user can access it
  /* 
  Below will check if user can access the object.  In general we will not want to check every object
     so checking all the resources shouldn't happen
  */
  $repo = tuque_wrapper_get_repository_instance();
  si_exhibition_debug('repo from si_exhibition_check_object_info_accessible');
  try {
    $object = $repo->getObject($object_info['pid']);
    // @todo remove object from cache.
  } catch(Exception $e) {
    return FALSE;
  }
  return TRUE;
}
/**
 * Determine if the object in question should be visible.
 *
 * $parent_pid_path - String array of pids
 *                  the path from the root of the exhibition to the direct parent of the pid to check
 *                  This needs to be a full path because concepts can show up in the exhibition more than once and have
 *                  different visibility settings on their children between each occurrance.
 *                  If NULL then will pull from the assumed path based on page request  
 * $to_check_pid - String of the pid to check for visibility
 * returns TRUE or FALSE
 *
 */
function si_exhibition_check_visibility($to_check_pid, $parent_pid_path = NULL) {
  $vis_obj = si_exhibition_get_visibility_object($to_check_pid, $parent_pid_path);
  $debug_info = array();
  $debug_info['tcp'] = $to_check_pid;
  $debug_info['ppp'] = $parent_pid_path;
  $debug_info['vo'] = $vis_obj;
  si_exhibition_debug('check_visibility', $debug_info);
  return ($vis_obj !== FALSE);
}
/**
 * $parent_pid_path - String array of pids
 *                  - the path from the root of the exhibition to the direct parent of the pid to check
 * $to_check_pid - String of the pid to check for visibility
 * This needs to be a full path because concepts can show up in the exhibition more than once and have
 * different visibility settings on their children between each occurrance.
 *
 */
function si_exhibition_get_visibility_object($to_check_pid, $parent_pid_path = NULL) {
  if ($parent_pid_path === NULL) {
    // Find where the path of the current concept from the exhibition root
    $parent_pid_path = si_exhibition_get_pid_path_to_showing_concept();
  }
  // Go to that place in the visibility tree
  // Return if the child pid is there
  if (!empty($parent_pid_path)){
    $vis_tree = si_exhibition_get_visibility_tree($parent_pid_path[0]);
    $curr_children = array($vis_tree);
    foreach($parent_pid_path as $curr_pid){
      $found_next_child = FALSE;
      $next_children = NULL;
      $curr_parent = NULL;
      foreach($curr_children as $curr_child) {
        if ($curr_child['pid'] == $curr_pid) {
          $found_next_child = TRUE;
          if (array_key_exists('children', $curr_child)) {
            $next_children = $curr_child['children'];
          }
          else {
            $next_children = array(); 
          }
          $curr_parent = $curr_child;
        }
      }
      if ($found_next_child) {
        $curr_children = $next_children;
      }
      else {
        return FALSE;
      }
    }
    $debug_info = array();
    $debug_info['cc'] = $curr_children;
    $debug_info['cp'] = $curr_parent;
    $debug_info['args'] = func_get_args();
    si_exhibition_debug('get_visibility_object', $debug_info);

    //If we're looking for a concept then we should have ended up at the concept
    if ($curr_parent['pid'] == $to_check_pid) {
      return $curr_parent;//array('show_meta'=>TRUE);//$curr_parent;
    }
    
    //If it's made it here, then it didn't return false when attempting to go down the wrong path
    //So the object that is located in $curr_children will hold all the valid objects that will be
    //visible
    foreach($curr_children as $curr_child){
      if ($curr_child['pid'] == $to_check_pid){
        return $curr_child;
      }
    }
    return FALSE;
  }
  else {
    //The parent path is empty, so we are looking for the root pid to match the checking_pid
    $vis_tree = si_exhibition_get_visibility_tree($to_check_pid);
    if (!empty($vis_tree) && !empty($vis_tree['pid'])){
      if ($vis_tree['pid'] == $to_check_pid) {
        return $vis_tree;
      }
    }
  }
  return FALSE;
}
/*
* For exhibitions, hide parents that aren't supposed to be shown, only the parents that should be in the current exhibition path
* Only used in the si_exhibition_get_parent_info function
*/
function si_exhibition_check_parent_in_tree(array $object_info, $tree_pid) {
  $tree_pid_list = array();
  $tree = _si_exhibition_get_pid_tree($tree_pid, null, $tree_pid_list);
  return in_array($object_info['pid'], $tree_pid_list);
}


/**
 * Fetchs some information about the given object's parents.
 *
 * As of 2016/02/15 this is only used when the exhibition is not in tree mode in order to show the parents in
 * the navigation
 *
 * @param string $pid
 *   The pid of the object who's parents info will be returned.
 *
 * @return array
 *   An array of array's containing the following key value pairs for each parent of the given object:
 *    pid, label
 */
function si_exhibition_get_parent_info_by_id($pid, $skipNonTreeParents = false) {
  $repo = tuque_wrapper_get_repository_instance();
  si_exhibition_debug('repo from si_exhibition_get_parent_info_by_id');
  $query = si_exhibition_generate_parent_concept_query($pid);
  //$results = $repo->ri->itqlQuery($query);
  $results = $repo->ri->sparqlQuery($query);
  $parents = array_map(function($o) {
    return array('pid' => $o['object']['value'], 'label' => $o['title']['value']); 
  }, $results);
  
  // @todo replace the checks for each objects existance once we have the appropriate information in the rels to determine if its accessible.
  if ($skipNonTreeParents) {
    $parents = array_filter($parents, function($elem){
      $path = (isset($_GET['path']) ? $_GET['path'] : '');
      if (!$path) {
        return false;
      }
      $paths = explode(',',$path);
      return si_exhibition_check_parent_in_tree($elem, $paths[0]);
    });
    if (variable_get('si_exhibition_anonymous_only', FALSE)){
      $parents = array_filter($parents, function($elem){
        return si_exhibition_check_view_permission($elem['pid']);
      });
    }
  }
  
  return array_filter($parents, 'si_exhibition_check_object_info_accessible');
}

/**
 * Query for fetching the Parent Concepts pids and labels.
 *
 * @param string $pid
 *   The object to query for.
 *
 * @return string
 *   An itql query string.
 */
function si_exhibition_generate_parent_concept_query($pid) {
  /*return "select ?object ?title from <#ri> where {
    ?object <fedora-model:state> <fedora-model:Active>;
            <fedora-model:label> ?title;
            <fedora-rels-ext:hasConcept> <info:fedora/" . $pid . ">
    } order by ?title";*/
	return "select ?object ?title from <#ri>
       where {
         ?object <info:fedora/fedora-system:def/model#state> <info:fedora/fedora-system:def/model#Active> .
         ?object <info:fedora/fedora-system:def/model#label> ?title .
         ?object <info:fedora/fedora-system:def/relations-external#hasConcept> <info:fedora/" . $pid . ">
     } ORDER BY ?title";
}

/**
 * Fetchs some information about the given object's child concepts.
 *
 * @param object_id
 *   Pid of the parent object who's child concept info will be returned.
 *
 * @return array
 *   An array of array's containing the following key value pairs for each child concpet of the given object:
 *    pid, label
 */
function si_exhibition_get_child_concept_info_by_id($object_id) {
  //Start cache check for return
  $cache_name = 'child_concept_info';
  $cache_index = $object_id;
  $to_return = si_exhibition_cache($cache_name,$cache_index);
  if (!empty($to_return)){ return $to_return; }
  si_exhibition_cron_cache(__FUNCTION__, func_get_args());
  //End cache

  $repo = tuque_wrapper_get_repository_instance();
  si_exhibition_debug('repo from si_exhibition_get_child_concept_info');
  $query = si_exhibition_generate_child_concept_query($object_id);
  $results = $repo->ri->sparqlQuery($query);
  $children = array_map(function($o) { return array('pid' => $o['o']['value'], 'label' => $o['t']['value'], 'count' => $o['k0']['value']); }, $results);
  // @todo replace the checks for each objects existance once we have the appropriate information in the rels to determine if its accessible.
  if (variable_get('si_exhibition_anonymous_only', FALSE)){
    $children = array_filter($children, function($elem){
      return si_exhibition_check_view_permission($elem['pid']);
    });
  }
  $children = array_filter($children, 'si_exhibition_check_object_info_accessible');

  $to_return = $children;
  return si_exhibition_cache($cache_name, $cache_index, $to_return);
}

/**
 * Query for fetching the Child Concepts labels and the number of Child Concepts they have.
 *
 * @param string $pid
 *   The object to query for.
 *
 * @return string
 *   An itql query string.
 */
function si_exhibition_generate_child_concept_query($pid) {
  /*$count = 'count(select $c from <#ri> where
    $o <fedora-rels-ext:hasConcept> $c and
    $c <fedora-model:state> <fedora-model:Active>)';
  return 'select $o $t ' . $count . ' from <#ri> where
     $o <fedora-model:state> <fedora-model:Active> and
     $o <fedora-model:label> $t and
     <info:fedora/' . $pid . '> <fedora-rels-ext:hasConcept> $o
     order by $t';*/
		 return "SELECT ?o ?t (COUNT(DISTINCT ?c) AS ?k0) FROM <#ri>
            WHERE {
              <info:fedora/" . $pid . "> <info:fedora/fedora-system:def/relations-external#hasConcept> ?o .
              ?o <info:fedora/fedora-system:def/model#state> <info:fedora/fedora-system:def/model#Active> .
              ?o <info:fedora/fedora-system:def/model#label> ?l .
              OPTIONAL {
                ?o <info:fedora/fedora-system:def/relations-external#hasConcept> ?c .
                ?c <info:fedora/fedora-system:def/model#state> <info:fedora/fedora-system:def/model#Active> .
              }
            }
            GROUP BY ?o (STR(?l) AS ?t)
            ORDER BY ?t";
}
/*
 * Enable verbose levels of debugging
 */
function si_exhibition_debug($message, $object = NULL){
  if (variable_get('si_exhibition_debug',FALSE)){
    drupal_set_message($message);
    if (!empty($object)){
      ob_start();
      print '<div id="" style="overflow-y: scroll; max-height:400px;">';
      print '<pre>';
      var_dump ($object);
      print '</pre>';
      print '</div>';
      $content = ob_get_clean();
      drupal_set_message($content);
    }
  }
}

/*
 * Returns the data from the cache if called with a cache_name and a cache_id
 * Returns the entire cached array if called with a cache_name, empty string and no third param
 * Returns the "overwrite" parameter if it has a value
 * Saves data in the cache if "overwrite" parameter has a value
 * If permanent set to true, will not expire
 */
function si_exhibition_cache($cache_name, $cache_data_index, $overwrite = FALSE, $permanent = FALSE){
  global $tnum;
  si_exhibition_debug("si_exhibition_cache $cache_name | $cache_data_index | $overwrite | $permanent |");
  if (!variable_get('si_exhibition_cache_on',TRUE)){
    si_exhibition_debug("si_exhibition_cache is turned off");
    if ($overwrite !== FALSE) return $overwrite; //If caching is turned off, don't use caching!
    return NULL;
  }
  $prefix = 'si_exhibition_cache_';
  $cache_table_for_clearing = 'cache';
  if ($overwrite === FALSE){
    $cache = NULL;
    if ($cache_info = cache_get($prefix.$cache_name, $cache_table_for_clearing)){
      $cache = $cache_info->data;
      if ($cache_data_index == ''){
        si_exhibition_debug("si_exhibition_cache returning entire cache", $cache);
        //return the entire cache
        return $cache;
      }
      if (array_key_exists($cache_data_index, $cache)){
        si_exhibition_debug("si_exhibition_cache returning specific cache", $cache[$cache_data_index]);
        return $cache[$cache_data_index];
      }
    }
  }else{
    $data = array();
    if ($cache_info = cache_get($prefix.$cache_name, $cache_table_for_clearing)){
      $data = $cache_info->data;
    }
    $data[$cache_data_index] = $overwrite;
    $hours = 3;
    $expire_date_time = time() + 60 * 60 * $hours;
    if ($permanent === TRUE){
      $expire_date_time = CACHE_PERMANENT;
    }
    si_exhibition_debug("si_exhibition_cache setting cache $prefix | $cache_name", $data);
    cache_set($prefix.$cache_name, $data, $cache_table_for_clearing, $expire_date_time);
    return $overwrite;
  }
  si_exhibition_debug("si_exhibition_cache returning nothing found in cache");
  return NULL;
}
/*
* Manipulate or get information from the cron_cache
*
* If no arguments are supplied, return the entire cron_cache
*
* If a function name is supplied, return the array of arrays of arguments for the function
*
* If function name and arguments are supplied,
* Put information into the cache that cron will use to update the other cached data
*/

function si_exhibition_cron_cache($function_name = FALSE, $arguments = FALSE){
  $current_cron_cache = si_exhibition_cache('cron_check','', FALSE, TRUE);
  if ($current_cron_cache == NULL) $current_cron_cache = array();
  si_exhibition_debug("si_exhibition_cron_cache $function_name");
  if ($function_name === FALSE) return $current_cron_cache;
  if (!array_key_exists($function_name, $current_cron_cache) || !is_array($current_cron_cache[$function_name])){
    $current_cron_cache[$function_name] = array(); 
  }
  if ($arguments === FALSE) return $current_cron_cache[$function_name];

  //Not a simple return, so it's a storage.  Already created the cache if it didn't exist yet
  //Now store the arguments (if they are not stored yet)
  if (!in_array($arguments, $current_cron_cache[$function_name])) $current_cron_cache[$function_name][] = $arguments;
  si_exhibition_cache('cron_check',$function_name, $current_cron_cache[$function_name], TRUE);
}
/*
 * Retrieve a path from the exhibition root to the current concept pid
 *
 * $url - optional parameter to give a path based on a specified url
 *        If empty or omitted, will attempt the current URL
 *        If current URL is not a valid concept display, will attempt referrer URL
 * Returns an array of pids that represent the path from the exhibition root to the currently displaying concept.
 *     The final entry in the array will be the currently showing concept
 *     If cannot get a proper path, will return an empty array
 * 
 */
function si_exhibition_get_pid_path_to_showing_concept($url = NULL) {
  if (!empty($url)) {
    return si_exhibition_get_pid_path_to_concept_by_url($url);
  }
  $pid_path = si_exhibition_get_pid_path_to_concept_by_url($_SERVER['REQUEST_URI']);
  if (!empty($pid_path)) {
    $_SESSION['si_exhibition_latest_pid_path'] = $pid_path;
    return $pid_path;
  }
  $pid_path = si_exhibition_get_pid_path_to_concept_by_url($_SERVER['HTTP_REFERER']);
  if (!empty($pid_path)) {
    $_SESSION['si_exhibition_latest_pid_path'] = $pid_path;
    return $pid_path;
  }
  if (!empty($_SESSION['si_exhibition_latest_pid_path'])){
    return $_SESSION['si_exhibition_latest_pid_path'];
  }
  return array();
}
/*
 * Return whether a string has the possibility of being a pid
 */
function si_exhibition_check_valid_pid($pid) {
      //Check that it's a valid pid: https://wiki.duraspace.org/display/FEDORA34/Fedora+Identifiers
  return (preg_match('/^([A-Za-z0-9]|-|\.)+:(([A-Za-z0-9])|-|\.|~|_|(%[0-9A-F]{2}))+$/',$pid) == 1);
}
/*
 * Breaks the URL up to return a path to the concept based on the following:
 * <server>/fedora/repository/<current_concept_pid>?<any request vars>path=<CSV_of_pid_path_starting_at_exhibition_root><any additional request vars>
 * 
 * Return an array of exhibition root to showing concept or empty array
 */
function si_exhibition_get_pid_path_to_concept_by_url($url) {
  $to_return = array();
  $url_info = parse_url($url);
  if ($url_info){
    $request_path = $url_info['path'];
    $fr_location = strpos($request_path,'/fedora/repository/');
    if ($fr_location !== FALSE) {
      $final_pid = urldecode(substr($request_path,$fr_location+19));
      if (si_exhibition_check_valid_pid($final_pid)) {
        if (!empty($url_info['query'])) {
          parse_str($url_info['query'],$query_holder);
          if (array_key_exists('path',$query_holder)){
            //path=<blank> is valid
            if (!empty($query_holder['path'])){
              $check_pid_path = explode(',',$query_holder['path']);
              foreach($check_pid_path as $check_pid) {
                if (!si_exhibition_check_valid_pid(urldecode($check_pid))){
                  print 'Invalid pid:'.$check_pid;
                  return $to_return; //Invalid pid in path
                }
              }
              $to_return = $check_pid_path;
            }
          }
        }
        $to_return[] = $final_pid;
      } 
    }
  }
  return $to_return;
}
/**
 * Fetchs some information about the given object's child resources.
 *
 * @param string $pid
 *   The object to query for.
 * @param string $type
 *   The type of resource to look for, can be any one of ('images', 'pdf', 'text', 'csv', 'video').
 * @param string $sort
 *   The sort to apply to the resources label either ('asc', 'desc').
 *
 * @return array
 *   An array of array's containing the following key value pairs for each child concpet of the given object:
 *    pid, label, datastreams
 */
function si_exhibition_get_child_resource_info(FedoraObject $object, $type = 'all', $sort = 'asc') {
  return si_exhibition_get_child_resource_info_by_id($object->id, $type, $sort);
}
function si_exhibition_get_child_resource_info_by_id($object_id, $type = 'all', $sort = 'asc') {

  //Start cache check for return
  $cache_name = 'child_resource_info';
  $cache_index = $object_id.'_'.$type.'_'.$sort;
  $to_return = si_exhibition_cache($cache_name,$cache_index);
  if (!empty($to_return)){ return $to_return; }
  si_exhibition_cron_cache(__FUNCTION__, func_get_args());
  //End cache

  $resource_types = array(
    'all' => array(),
    'images' => array('si:imageCModel', 'si:generalImageCModel'),
    'pdf' => array('si:fieldbookCModel'),
    'csv' => array('si:datasetCModel'),
    'text' => FALSE,
    'video' => array('islandora:sp_videoCModel'),
		'audio' => array('islandora:sp-audioCModel')
  );
  $resources = array();
  $resource_types = isset($resource_types[$type]) ? $resource_types[$type] : FALSE;
  $parent_path = si_exhibition_get_pid_path_to_showing_concept();
  array_pop($parent_path); //Remove the current concept from the "parents" list
  $vis_obj = si_exhibition_get_visibility_object($object_id, $parent_path); //Get the visibility of the current concept

  //Walk through the visibility pulling out the type and number of items asked for
  $count_so_far = 0;
  $resources = array();
  if (empty($vis_obj)){
    //The visibility object needs to exist to pull info from it
    return NULL;
  }
  if (array_key_exists('children', $vis_obj)) {
    foreach($vis_obj['children'] as $child_vis_obj) {
      if (
          !empty($child_vis_obj['pid']) && 
          !empty($child_vis_obj['show_name']) && 
          !empty($child_vis_obj['type']) && 
          !empty($child_vis_obj['model']) && 
          !empty($child_vis_obj['label']) && 
          (strtolower($child_vis_obj['type']) == 'resource')
         ) {
        $add_current_resource = FALSE;
        foreach($resource_types as $user_selected_resource_model) {
          $current_obj_model = $child_vis_obj['model'];
          if ($current_obj_model == $user_selected_resource_model) {
            $add_current_resource = TRUE;
          }
        }
        if (empty($resource_types)) {  //Indicates "All" is selected
          $add_current_resource = TRUE; 
        }
        if ($add_current_resource) {
          $resources[] = array('pid' => $child_vis_obj['pid'], 'label' => $child_vis_obj['label']);
        }
      }
    }
  }

  $resources =  array_filter($resources, 'si_exhibition_check_object_info_accessible'); //BBB Security changes will necessitate this being used
  /*
  $visible_child_list = array();
  if (!empty($vis_obj) && !empty($vis_obj['children'])) {
    //Fill up the list of items that the current concept has as visible children
    foreach($vis_obj['children'] as $child_vis_obj) {
      if (
          !empty($child_vis_obj['pid']) && 
          !empty($child_vis_obj['show_name']) && 
          !empty($child_vis_obj['type']) && 
          ($child_vis_obj['type'] == 'resource')
         ) {
        $visible_child_list[] = $child_vis_obj['pid'];
      }
    }
  }
  //Only return resources that should be visible
  $resources = array_filter($resources, function($elem) use($visible_child_list){
     return in_array($elem['pid'],$visible_child_list);
  });
  */
  $to_return = $resources;
  return si_exhibition_cache($cache_name, $cache_index, $to_return);
}
/*
 * Used to fill out the information of the specific resources that will be shown on screen
 * The resources should already be filtered and visible
 * $resources - array of resources which minimally each element has ['pid']
 * $show - unused
 * $limit - how many to show out of the $resources array
 * $offset - first element to fill information for
 */
function si_exhibition_fill_out_resources(&$resources, $show, $limit = NULL, $offset = NULL){
  if ($resources === NULL) return; //It should at least be an empty array if no resources shown
  if (empty($limit)) $limit = sizeof($resources);
  if (empty($offset)) $offset = 0;
  $repo = tuque_wrapper_get_repository_instance();
  si_exhibition_debug('repo from si_exhibition_fill_out_resources');
  foreach ($resources as $key => $resource) {
     if ($key >= $offset && $key <= $offset+$limit){
      $object = $repo->getObject($resource['pid']); // We know it exists from the previous filter call.
      $resources[$key]['models'] = $object->models;
      $resources[$key]['datastreams'] = array();
      try {
        foreach ($object as $dsid => $datatream) {
          $resources[$key]['datastreams'][] = $dsid;
        }
      } catch(Exception $e) { 
        //unset($resources[$key]); 
      } // Ignore when we can't access a resources datastream.
      if (variable_get('si_exhibition_anonymous_only', FALSE)){
        if (!si_exhibition_check_view_permission($resource['pid'])){
          unset($resources[$key]);
        }
      }
      $visibility_obj = si_exhibition_get_visibility_object($resource['pid']);
      $resources[$key]['visibility'] = $visibility_obj;
    }
  }
}
/**
 * Query for fetching the Child Resources given what type of data they disseminate.
 *
 * @param string $pid
 *   The object to query for.
 * @param string $resource_types
 *   The types of resource to look for, defined as a list of content models, if empty all resources will be returned.
 * @param string $sort
 *   The sort to apply to the resources label either ('asc', 'desc').
 * @param int $limit
 *   The number of results to limit this query to.
 * @param int $offset
 *   The offset into the number of results to truncate from the results.
 *
 * @return string
 *   An itql query string
 */

function si_exhibition_generate_child_resource_query($pid, array $resource_types, $sort = 'asc', $limit = NULL, $offset = NULL) {
  $limit_types = '';
  if (!empty($resource_types)) {
    $limit_type_expression = function($o) {
      return "sameTerm(?model, <info:fedora/$o>)";
    };
    $limit_types = array_map($limit_type_expression, $resource_types);
    $limit_types = implode(' || ', $limit_types);
    $limit_types = "({$limit_types}) && ";
  }
  $limit = isset($limit) ? " $limit" : '1000000';
  $offset = isset($offset) ? " $offset" : '0';
  $sort = strtoupper($sort);
  /*$to_return = "select ?object ?title ?model from <#ri> where {
    ?object <fedora-model:state> <fedora-model:Active>;
            <fedora-model:label> ?title;
            <fedora-model:hasModel> ?model.
    <info:fedora/{$pid}> <fedora-rels-ext:hasResource> ?object;
    FILTER(
		" . $limit_types . "
       !sameTerm(?model, <info:fedora/si:resourceCModel>) &&
			 !sameTerm(?model, <info:fedora/fedora-system:FedoraObject-3.0>))
		}	 
    ORDER BY {$sort}(?title)
		LIMIT {$limit}
		OFFSET {$offset}";*/
		$to_return = "select ?object ?title ?model from <#ri> where {
          <info:fedora/" . $pid . "> <info:fedora/fedora-system:def/relations-external#hasResource> ?object .
          ?object <info:fedora/fedora-system:def/model#state> <info:fedora/fedora-system:def/model#Active> .
          ?object <info:fedora/fedora-system:def/model#label> ?title .
          ?object <info:fedora/fedora-system:def/model#hasModel> ?model;
          FILTER(
          ".$limit_types."
              !sameTerm(?model, <info:fedora/fedora-system:FedoraObject-3.0>) &&
              !sameTerm(?model, <info:fedora/si:resourceCModel>))
        }
        ORDER BY {$sort}(?title)
        OFFSET {$offset}
        LIMIT {$limit}";
	si_exhibition_debug('<pre>'.htmlspecialchars($to_return).'</pre>');
  return $to_return;
}
/*
*/

/**
 * Fetches the get paramters to configure the pager.
 *
 * @return array
 *   array($show, $sort, $limit, $offset)
 */
function si_exhibition_get_pager_parameters() {
  $show = isset($_GET['show']) ? $_GET['show'] : 'all';
  $sort = isset($_GET['sort']) ? $_GET['sort'] : 'asc';
  $limit = isset($_GET['limit']) ? $_GET['limit'] : 10;
  $page =  isset($_GET['page']) ? $_GET['page'] : 0;
  return array($show, $sort, $limit, $page);
}

/**
 * Fetchs the given objects datastream contents.
 * Fails silently if something goes wrong.
 *
 * @param string $pid
 *   The PID of the object.
 * @param string $dsid
 *   The DSID of the datastream to fetch from.
 *
 * @return string
 *   The datastream's contents if successful, an empty string otherwise.
 */
function si_exhibition_get_datastream_contents(FedoraObject $object, $dsid) {
  if (isset($object[$dsid])) {
    $repo = tuque_wrapper_get_repository_instance();
    si_exhibition_debug('repo from si_exhibition_get_datastream_contents');
    return $repo->api->a->getDatastreamDissemination($object->id, $dsid, NULL); // Work around for bug in Tuque API: ISLANDORA-752
  }
  return '';
}

