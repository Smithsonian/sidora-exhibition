<?php
/**
 * @file
 *
 * Template file for si_exhibition object displays
 *
 * Variables available:
 * - $navigation:
 * - $title:
 * - $metada:
 * - $resources:
 */

// $object is no longer returned directly.
// It has been saved into $variables though.
$object = $variables['object'];
$models = implode(' ', $object->models);
$model_classes = strtolower(preg_replace('/[^A-Za-z0-9 ]/', '-', $models) ); // Make valid css class
$variables['pid'] = $object->id;
?>
<div id="si-exhibition" class="<?php print $layout; ?>">
   <?php if ($block_exposed == 'no'): ?>
     <?php print theme('si_exhibition_navigation', array('pid' => $object->id)); ?>
   <?php endif; ?>
   
   
   <div id="si-content">
     <h2 class="object-title"><?php print theme('si_exhibition_page_title', array('label' => $object->label)); ?></h2>
     <span class="si-icon <?php 
     print $model_classes; 
     ?>"></span>
     <div id="si-content-inner">
      <?php print theme('si_exhibition_metadata_xmls', $variables); 
      
      ?>
     </div>
   </div>
   <div id="si-resources">
    <h2><?php print t('Resources'); ?></h2>
    <div id="si-resources-inner">
			<?php print theme('si_exhibition_resources', $variables); ?>
    </div>
  </div>
</div>
