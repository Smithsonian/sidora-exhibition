<?php

/**
 * @file
 *
 * This module's Admin settings form.
 */

/**
 * Defines this modules admin settings form.
 *
 * @param array $form_state
 *   The Drupal form state.
 *
 * @return array
 *   The drupal form definition.
 */
function si_exhibition_admin_settings($form, &$form_state) {
  $form = array(
    'si_exhibition_expose_block' => array(
      '#type' => 'select',
      '#title' => t('Expose navigation as block'),
      '#default_value' => variable_get('si_exhibition_expose_block', 'no'),
      '#options' => array(
        'no' => t('No'),
        'combined' => t('Combined'),
        'split' => t('Split'),
      ),
      '#description' => t('No: Don\'t expose navigation as block. Navigation will be part of the object layout in the content area.<br/>Combined: Expose navigation in one block. <br/>Split: Expose Parent and Child concept navigation in two separate blocks.<br/>Place the block(s) in the preferred region(s) <a href="@url">here</a>.', array(
                        '@url' => url('admin/build/block/list')
                      )
      ),
    ),
    'si_exhibition_data_url' => array(
      '#type' => 'textfield',
      '#title' => t('Where to obtain the website configuration for the page (not where to obtain the Fedora data itself)'),
      '#default_value' => variable_get('si_exhibition_data_url', ''),
      '#description' => t('The Fedora Commons pid will be directly appended to the above, end with / if needed.'),
    ),
    'si_exhibition_metadata_xsl' => array(
      '#type' => 'textarea',
      '#rows' => '25',
      '#title' => t('XSL to change the metadata sets of the Fedora Items into a metadata table'),
      '#default_value' => variable_get('si_exhibition_metadata_xsl',si_exhibition_xsl_string_default()),
    ),
    'si_exhibition_anonymous_only' => array(
      '#type' => 'checkbox',
      '#title' => t('Only allow items to appear in exhibitions that have full anonymous read access.'),
      '#default_value' => variable_get('si_exhibition_anonymous_only',FALSE),
    ),
    'fedora_base_url' => array(
      '#type' => 'textfield',
      '#title' => t('Where to obtain the Fedora Commons objects'),
      '#default_value' => variable_get('fedora_base_url', 'http://localhost:8080/fedora'),
      '#description' => t('The Fedora Commons Repo location'),
    )
  );
  return system_settings_form($form);
}

