<?php
/**
 * @file islandora-solr-custom.tpl.php
 * Islandora solr search results template
 *
 * Variables available:
 * - $variables: all array elements of $variables can be used as a variable. e.g. $base_url equals $variables['base_url']
 * - $base_url: The base url of the current website. eg: http://example.com .
 * - $user: The user object.
 *
 * - $style: the style of the display ('div' or 'table'). Set in admin page by default. Overridden by the query value: ?display=foo
 * - $results: the array containing the solr search results
 * - $table_rendered: If the display style is set to 'table', this will contain the rendered table.
 *    For theme overriding, see: theme_islandora_solr_custom_table()
 * - $switch_rendered: The rendered switch to toggle between display styles
 *    For theme overriding, see: theme_islandora_solr_custom_switch()
 *
 */
?>
<?php print $switch_rendered; ?>
<?php if ($style == 'div'): ?>
  <ol class="islandora_solr_results" start="<?php print $record_start; ?>">
    <?php if ($results == ''): print '<p>' . t('Your search yielded no results') . '</p>'; ?>
    <?php else: ?>
      <?php foreach ($results as $id => $result): ?>
        <li class="islandora_solr_result">
          <?php $pid_link = 'fedora/repository/' . $result['PID']['value']; ?>
          <?php if (!empty($result['DC.content.title_s']['value'])): ?>
            <div class="solr-field <?php print $result['DC.content.title_s']['class']; ?>">
              <div class="label"><label><?php print t($result['DC.content.title_s']['label']); ?>:</label></div>
              <div class="value"><?php print l($result['DC.content.title_s']['value'], $pid_link); ?></div>
            </div>
          <?php endif; ?>
          <?php if (!empty($result['EAC-CPF.content.topic_s']['value'])): ?>
            <div class="solr-field <?php print $result['EAC-CPF.content.topic_s']['class']; ?>">
              <div class="label"><label><?php print t($result['EAC-CPF.content.topic_s']['label']); ?>:</label></div>
              <div class="value"><?php print $result['EAC-CPF.content.topic_s']['value']; ?></div>
            </div>
          <?php endif; ?>
          <?php if (!empty($result['EAC-CPF.content.place_s']['value'])): ?>
            <div class="solr-field <?php print $result['EAC-CPF.content.place_s']['class']; ?>">
              <div class="label"><label><?php print t($result['EAC-CPF.content.place_s']['label']); ?>:</label></div>
              <div class="value"><?php print $result['EAC-CPF.content.place_s']['value']; ?></div>
            </div>
          <?php endif; ?>
          <?php if (!empty($result['EAC-CPF.content.person_s']['value'])): ?>
            <div class="solr-field <?php print $result['EAC-CPF.content.person_s']['class']; ?>">
              <div class="label"><label><?php print t($result['EAC-CPF.content.person_s']['label']); ?>:</label></div>
              <div class="value"><?php print $result['EAC-CPF.content.person_s']['value']; ?></div>
            </div>
          <?php endif; ?>
          <?php if (!empty($result['EAC-CPF.content.agencyName_s']['value'])): ?>
            <div class="solr-field <?php print $result['EAC-CPF.content.agencyName_s']['class']; ?>">
              <div class="label"><label><?php print t($result['EAC-CPF.content.agencyName_s']['label']); ?>:</label></div>
              <div class="value"><?php print $result['EAC-CPF.content.agencyName_s']['value']; ?></div>
            </div>
          <?php endif; ?>
          <?php if (!empty($result['EAC-CPF.content.corporateBody_s']['value'])): ?>
            <div class="solr-field <?php print $result['EAC-CPF.content.corporateBody_s']['class']; ?>">
              <div class="label"><label><?php print t($result['EAC-CPF.content.corporateBody_s']['label']); ?>:</label></div>
              <div class="value"><?php print $result['EAC-CPF.content.corporateBody_s']['value']; ?></div>
            </div>
          <?php endif; ?>
          <?php if (!empty($result['MODS.content.topic_s']['value'])): ?>
            <div class="solr-field <?php print $result['MODS.content.topic_s']['class']; ?>">
              <div class="label"><label><?php print t($result['MODS.content.topic_s']['label']); ?>:</label></div>
              <div class="value"><?php print $result['MODS.content.topic_s']['value']; ?></div>
            </div>
          <?php endif; ?>

          <?php if (!empty($result['MODS.content.creator_s']['value'])): ?>
            <div class="solr-field <?php print $result['MODS.content.creator_s']['class']; ?>">
              <div class="label"><label><?php print t($result['MODS.content.creator_s']['label']); ?>:</label></div>
              <div class="value"><?php print $result['MODS.content.creator_s']['value']; ?></div>
            </div>
          <?php endif; ?>
          <?php if (!empty($result['DC.content.description_s']['value'])): ?>
          <?php
          $readmore = '';
          $max_length = 500;
          if (strlen($result['DC.content.description_s']['value']) > $max_length) {
            $result['DC.content.description_s']['value'] = preg_replace('/\s+?(\S+)?$/', '', substr($result['DC.content.description_s']['value'], 0, $max_length));
            $readmore = '... (' . l(t('Read More'), $pid_link) . ')';
          }
          ?>
            <div class="solr-field <?php print $result['DC.content.description_s']['class']; ?>">
              <div class="label"><label><?php print t($result['DC.content.description_s']['label']); ?>:</label></div>
              <div class="value"><?php print $result['DC.content.description_s']['value'] . $readmore; ?></div>
            </div>
          <?php endif; ?>
        </li>
      <?php endforeach; ?>
    <?php endif; ?>
  </ol>
<?php elseif ($style == 'table'): ?>
  <?php print $table_rendered; ?>
<?php endif; ?>