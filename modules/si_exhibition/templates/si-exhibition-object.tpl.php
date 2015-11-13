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
?>
<div id="si-exhibition" class="<?php print $layout; ?>">
   <?php if ($block_exposed == 'no'): ?>
     <?php print theme('si_exhibition_navigation', array('object' => $object)); ?>
   <?php endif; ?>
   
   
   <div id="si-content">
     <h2 class="object-title"><?php print theme('si_exhibition_page_title', array('object' => $object)); ?></h2>
     <span class="si-icon <?php print $model_classes; ?>"></span>
     <div id="si-content-inner">
      <?php print theme('si_exhibition_metadata_xmls', $variables); ?>
     </div>
   </div>
   <div id="si-resources">
    <h2><?php print t('Resources'); ?></h2>
    <div id="si-resources-inner">
			<?php print theme('si_exhibition_resources', $variables); ?>
    </div>
  </div>
</div>
