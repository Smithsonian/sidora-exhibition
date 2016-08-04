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
