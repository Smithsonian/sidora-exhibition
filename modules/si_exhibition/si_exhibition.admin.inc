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
  module_load_include('inc', 'si_exhibition', 'includes/workbench_viewers.admin');
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
      '#size' => 100,
      '#description' => t('The Fedora Commons pid will be directly appended to the above, end with / if needed.'),
    ),
    'si_exhibition_workbench_xsl' => array(
      '#type' => 'textfield',
      '#title' => t('Where to obtain the XSL information from the workbench settings'),
      '#default_value' => variable_get('si_exhibition_workbench_xsl', ''),
      '#size' => 100,
      '#description' => t('The Fedora Commons pid will be directly appended to the above, end with / if needed.'),
    ),
    'si_exhibition_visibility_info_url' => array(
      '#type' => 'textfield',
      '#title' => t('Where to obtain the visibility information from the exhibition_config settings'),
      '#default_value' => variable_get('si_exhibition_visibility_info_url', ''),
      '#size' => 100,
      '#description' => t('The Fedora Commons pid will be directly appended to the above, end with / if needed. WARNING: If empty, will not show any exhibition.'),
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
    'si_exhibition_cache_on' => array(
      '#type' => 'checkbox',
      '#title' => t('Cache items for speed improvements.'),
      '#default_value' => variable_get('si_exhibition_cache_on',TRUE),
    ),
    'si_exhibition_debug' => array(
      '#type' => 'checkbox',
      '#title' => t('Turn on debugging mode.'),
      '#default_value' => variable_get('si_exhibition_debug',FALSE),
    ),
    'fedora_base_url' => array(
      '#type' => 'textfield',
      '#title' => t('Where to obtain the Fedora Commons objects'),
      '#default_value' => variable_get('fedora_base_url', 'http://localhost:8080/fedora'),
      '#description' => t('The Fedora Commons Repo location'),
    ),
    'sidora_js_lib_location_jstree' => array(
      '#type' => 'textfield',
      '#title' => t('jstree'),
      '#default_value' => variable_get('sidora_js_lib_location_jstree', ''),
      '#description' => t('This should be an http / https location containing jstree.js and themes/default/style.min.css.  jsTree can be downloaded from https://github.com/vakata/jstree or http://jstree.com'),
      '#attributes' => array('size' => 120),
      '#maxlength' => 1024,
    ),
  );
  _sidora_admin_settings_add_viewer_configs($form);
  return system_settings_form($form);
}

