<?php

function sidora_view_openseadragon($pid, $dsid, $version='0'){
  $osd_js_path = variable_get('osd_library_location', $GLOBALS['base_url'].'/sites/all/libraries/openseadragon-djatoka/');
  $local_resolver = variable_get('osd_local_resolver_service', $GLOBALS['base_url'].'/'.'sidora/osd/');
  $jquery = variable_get('osd_jquery_library_location', $GLOBALS['base_url'].'/sites/all/libraries/openseadragon-djatoka/jquery/jquery-2.1.4.min.js');
  print "\r\n".'<html>                                                                                                      ';
  print "\r\n".'  <head>                                                                                                    ';
  print "\r\n".'    <script src="'.$jquery.'"></script>';
  print "\r\n".'    <script src="'.$osd_js_path.'openseadragon/openseadragon.min.js"></script>';
  print "\r\n".'    <script src="'.$osd_js_path.'js/djtilesource.js"></script>';
  print "\r\n".'  </head>                                                                                                   ';
  print "\r\n".'  <body>                                                                                                    ';
  print "\r\n".'    <div id="container">                                                                                    ';
  print "\r\n".'      <div id="page-image" style="width: 100%; height: 100%;">                                            ';
  print "\r\n".'        <script>                                                                                            ';
  print "\r\n".'          <!-- create the tile source -->                                                                   ';
//  print "\r\n".'          var service = "/adore-djatoka/resolver";';
  //print "\r\n".'          var service = "'.$local_resolver.$pid.'/'.$dsid.'/'.$version.'?t='.time().'";';
  print "\r\n".'          var service = "'.$local_resolver.$pid.'/'.$dsid.'/'.$version.'";';
  print "\r\n".'          var id = "{replacer}";';  //replacer is a value that is hard-coded to be replaced with the proper id in the osd_proxy function
  print "\r\n".'          var settings = {tileSize: 256, tileOverlap: 0};                                                   ';
  print "\r\n".'          var tileSource = new OpenSeadragon.DjatokaTileSource(service, id, settings);                      ';
  print "\r\n".'          <!-- create the OpenSeadragon Viewer with the TileSource -->                                      ';
  print "\r\n".'          OpenSeadragon({                                                                                   ';
  print "\r\n".'            id: "page-image",                                                                               ';
  print "\r\n".'            prefixUrl: "'.$osd_js_path.'openseadragon/images/",';
  print "\r\n".'            showNavigator: true,                                                                            ';
//  print "\r\n".'            debugMode: '.time().',                                                                            ';
  print "\r\n".'            debugMode: false,                                                                                ';
  print "\r\n".'            tileSources: [tileSource],                                                                      ';
  print "\r\n".'            showRotationControl: true                                                                       ';
  print "\r\n".'          });                                                                                               ';
  print "\r\n".'        </script>                                                                                           ';
  print "\r\n".'      </div>                                                                                                ';
  print "\r\n".'    </div>                                                                                                  ';
  print "\r\n".'  </body>                                                                                                   ';
  print "\r\n".'</html>                                                                                                     ';
  drupal_exit();
}
/*
 * Returns the full view for the phpexcelreader plus javascript
 */
function sidora_tabular_view_phpexcelreader($pid,$dsid = 'OBJ'){
  require dirname(__FILE__).'/utils.inc';
  ob_start();
  print "Starting tabular viewer with pid:".$pid." and dsid:".$dsid;
  $debug_result = ob_get_clean();
  si_exhibition_debug($debug_result);

  $object = sidora_obj($pid);
  $datastream_content = $object[$dsid]->content;
  $datastream = $object[$dsid];
  $mime_type = (empty($datastream))?'text/xml':$datastream->mimeType;
  $file_extension = '';
  if ($mime_type != ''){
    $file_extension = sidora_get_file_extension_from_mime($mime_type);
  } 
  $filename = $pid.'_'.$dsid.$file_extension;//get_suggest_filename($label, $mime_type);
  $filename = str_replace(":","",$filename);
  $file_uri = drupal_tempnam(file_directory_temp(), $filename);
  $file = new stdClass();
  $file->uri = $file_uri;
  $file->filename = $filename;
  $file->filemime = $mime_type;
  $file->status = 0; // Temporary file.

  ob_start();
  print "About to save the first file:". file_directory_temp() . '/'.$filename;
  $debug_result = ob_get_clean();
  si_exhibition_debug($debug_result);

  file_unmanaged_save_data($datastream_content, file_directory_temp() . '/'.$filename, FILE_EXISTS_REPLACE); 

  ob_start();
  print "Saved first file:". file_directory_temp() . '/'.$filename;
  $debug_result = ob_get_clean();
  si_exhibition_debug($debug_result);

  $temp = file_directory_temp();
  $rels = $object->relationships->get(FEDORA_RELS_EXT_URI, 'hasCodebook');
  global $base_url;
  $data_tables_lib_location = variable_get('sidora_dataTables_location',$base_url.'/sites/all/libraries/dataTables/');
  echo '<html>
<head>
  <link rel="stylesheet" href="'.$data_tables_lib_location.'css/jquery.dataTables.css"></link>
  <script src="'.$data_tables_lib_location.'js/jquery.js"></script>
  <script src="'.$data_tables_lib_location.'js/jquery.dataTables.min.js"></script>
  <style>
     #hasHeader { display:none }
     #noHeader { display:none }
     #hasHeader_wrapper { display:none }
     #noHeader_wrapper { display:none }
  </style>
  <script>
    jQuery(document).ready(function(){
      jQuery("table").DataTable({
        "lengthMenu": [ [-1,5,10,25,100],["All",5,10,25,100] ],
        "order": []
      });
      jQuery("table").show();
      jQuery("#noHeader_wrapper").show();
      jQuery("#showHeaderTable").click(function(){
        jQuery("#noHeader_wrapper").toggle();
        jQuery("#hasHeader_wrapper").toggle();
      });
    });
  </script>
';

  ob_start();
  print "Echo 1 to browser complete";
  $debug_result = ob_get_clean();
  si_exhibition_debug($debug_result);

  $use_codebook_headers = '';

  ob_start();
  print "Checking RELS";
  $debug_result = ob_get_clean();
  si_exhibition_debug($debug_result);


  if (!empty($rels)){

    ob_start();
    print "RELS is true";
    $debug_result = ob_get_clean();
    si_exhibition_debug($debug_result);

    $codebook_pid = ($rels[0]['object']['value']);
    $codebook_obj = sidora_obj($codebook_pid);
    if (!empty($codebook_obj['DDI'])){

      ob_start();
      print "DDI is true";
      $debug_result = ob_get_clean();
      si_exhibition_debug($debug_result);

      $doc = new DOMDocument();
      $content = $codebook_obj['DDI']->content;
      $doc->loadXML($content);

      ob_start();
      print "DDI XML loaded";
      $debug_result = ob_get_clean();
      si_exhibition_debug($debug_result);

      $label_elements = $doc->getElementsByTagName('label');
      $labels = array(); 

      ob_start();
      print "DDI XML labels";
      var_dump ($label_elements);
      $debug_result = ob_get_clean();
      si_exhibition_debug($debug_result);

      foreach($label_elements as $element){
        $labels[] = $element->nodeValue;
      }
      $use_codebook_headers = '<br/><input type="checkbox" id="useCodebookHeaders" />Use Codebook Headers';
      echo '
  <script>
    jQuery(document).ready(function(){
      window.tablesInfo = {};
      tablesInfo.originalHasHeader = [];
      tablesInfo.codebookLabels = [';
      foreach($labels as $i => $label){
        if ($i > 0) echo ',';
        echo '"'.$label.'"';
      }
      echo '
      ];
      jQuery("#hasHeader th").each(function(index,value){
        tablesInfo.originalHasHeader.push(jQuery(value).text());
      });
      jQuery("#useCodebookHeaders").click(function(){
        if (jQuery("#useCodebookHeaders").prop("checked")){
          jQuery("#noHeader th").each(function(index,value){
            if (tablesInfo.codebookLabels.length > index){
              value.innerHTML = tablesInfo.codebookLabels[index];
            }else{
              value.innerHTML = "";
            }
          });
          jQuery("#hasHeader th").each(function(index,value){
            if (tablesInfo.codebookLabels.length > index){
              value.innerHTML = tablesInfo.codebookLabels[index];
            }else{
              value.innerHTML = "";
            }
          });
        }else{
          jQuery("#noHeader th").each(function(index,value){
            value.innerHTML = "";
          });
          jQuery("#hasHeader th").each(function(index,value){
            if (tablesInfo.originalHasHeader.length > index){
              value.innerHTML = tablesInfo.originalHasHeader[index];
            }else{
              value.innerHTML = "";
            }
          });
        } 
      });
    });
  </script>
';
    }
  }

      ob_start();
      print "Echo 2 to browser complete";
      $debug_result = ob_get_clean();
      si_exhibition_debug($debug_result);

  echo '
</head>
<body style="background:white;margin:20px;">
';
  echo '<input type="checkbox" id="showHeaderTable" />Has Header Row';
  echo $use_codebook_headers;

      ob_start();
      print "Tabular 1 outputting...".$temp.'/'.$filename;
      $debug_result = ob_get_clean();
      si_exhibition_debug($debug_result);

  echo sidora_tabular_view_phpexcelreader_html($temp.'/'.$filename, TRUE, 'hasHeader');

      ob_start();
      print "Tabular 2 outputting...".$temp.'/'.$filename;
      $debug_result = ob_get_clean();
      si_exhibition_debug($debug_result);

  echo sidora_tabular_view_phpexcelreader_html($temp.'/'.$filename, FALSE, 'noHeader');
  echo '</body>
</html>';

      ob_start();
      print "Echo final to browser complete";
      $debug_result = ob_get_clean();
      si_exhibition_debug($debug_result);

  drupal_exit();
}

/*
 * Returns the pure HTML table based on the given filename
 */
function sidora_tabular_view_phpexcelreader_html($filename, $first_line_header = TRUE, $table_id = ''){

  $to_return = '';
try{
      ob_start();
      print "Loading PHPExcel IOFactory";
      $debug_result = ob_get_clean();
      si_exhibition_debug($debug_result);

  
  require_once(libraries_get_path('PHPExcel') . '/Classes/PHPExcel/IOFactory.php');

      ob_start();
      print "Loading PHPExcel File:".$filename;
      $debug_result = ob_get_clean();
      si_exhibition_debug($debug_result);

  $objPHPExcel = PHPExcel_IOFactory::load($filename);

      ob_start();
      print "Getting active sheet of file:".$filename;
      $debug_result = ob_get_clean();
      si_exhibition_debug($debug_result);

  $worksheet = $objPHPExcel->getActiveSheet();
  $table_header = '<thead><tr>';
  $table_data = '<tbody>';
  $is_first = TRUE;
  $count = 0;

      ob_start();
      print "Iterating rows...".$filename;
      $debug_result = ob_get_clean();
      si_exhibition_debug($debug_result);

  foreach ($worksheet->getRowIterator() as $row) {
  	if (!$is_first || !$first_line_header){
      $table_data .= '<tr>';// class="'.(count%2==0)?'odd':'even'.'">';
    }
    $cellIterator = $row->getCellIterator();
  	$cellIterator->setIterateOnlyExistingCells(false); // Loop all cells, even if it is not set
    foreach ($cellIterator as $cell) {
      if ($is_first){
        if ($first_line_header){
          $table_header .= '<th>' . $cell->getValue() . '</th>';
        }else{
          $table_header .= '<th></th>'; 
        }
      }
      if (!is_null($cell) && !($is_first && $first_line_header)) {
        $table_data .= '<td>' . $cell->getValue() . '</td>';
      }
    }
  	if (!$is_first || !$first_line_header){
		  $table_data .= '</tr>';
    }
    $is_first = FALSE;
  }

      ob_start();
      print "Row iterating complete.".$filename;
      $debug_result = ob_get_clean();
      si_exhibition_debug($debug_result);

  $table_header .= '</tr></thead>';
  $table_data .= '</tbody>';

  $to_return = '<table class="display" id="'.$table_id.'">';
  $to_return .= $table_header;
  $to_return .= $table_data;
  $to_return .= '</table>';

      ob_start();
      print "Returning full html table.".$filename;
      $debug_result = ob_get_clean();
      si_exhibition_debug($debug_result);
}catch(Exception $e){
      ob_start();
      print "\n<br>\n";
      print $e->getMessage();
      print "\n<br>\n";
      var_dump($e);
      $debug_result = ob_get_clean();
      watchdog("PHPExcelViewer",$debug_result);
}
  return $to_return;
}
