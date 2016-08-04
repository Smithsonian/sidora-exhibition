<div class="hidden" style="display:none;">
<div class="controls">
   <?php print theme('item_list', array('items'=>$limit_types, 'title'=>t('Limit by type'), 'type'=>'ul', 'attributes'=>array('class' => 'limit-by-type'))) ?>
</div>
<div class="controls">
   <?php print  theme('item_list', array('items'=>$results_per_page, 'title'=>t('Results per page'), 'type'=>'ul', 'attributes'=>array('class' => 'results-per-page'))); ?>
</div>
</div>
<?php
$toPrintForm = drupal_get_form('exhibition_resources_form');
print render($toPrintForm);
?>

<div class="showing">
   <?php print t('Showing') . ' ' . ($offset + 1) . ' - ' . min($offset + $limit, $total)  . ' of ' . $total; ?>
</div>
<?php print $pager ?>
<?php print $table; ?>
<?php print $pager ?>
