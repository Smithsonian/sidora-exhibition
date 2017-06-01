<?php
if (!isset($variables['sidora_exhibition']['logo'])) {
  $variables['sidora_exhibition']['logo'] = '';
}
?>

<?php
$extra_css = (isset($variables['sidora_exhibition']['extra_css'])?$variables['sidora_exhibition']['extra_css']:''); 
if (!empty($extra_css)){
 print "<style>";
 print "\n body, a, a:link, a:visited { background-color:rgba(0,0,0,0.0); color: rgba(255,255,255);} \n";
 print $extra_css;
 print "</style>";
}
?>


  <div id="page" class="page">
    <div id="page-inner" class="page-inner">
      <div id="skip">
        <a href="#main-content-area" class="element-invisible element-focusable"><?php print t('Skip to Main Content Area'); ?></a>
      </div>

      <!-- header-group row: width = grid_width -->
      <div id="header-group-wrapper" class="header-group-wrapper full-width">
        <div id="header-group" class="header-group row <?php print $grid_width; ?>">
          <div id="header-group-inner" class="header-group-inner inner clearfix">
            <?php //print theme('grid_block', array('content' => theme('links', $secondary_links), 'id' => 'secondary-menu')); ?>
            
            <?php if (isset($search_box)): ?>
              <div class="topbar">
                <?php print theme('grid_block', array('content' => $search_box, 'id' => 'search-box')); ?>
              </div>
            <?php endif; ?>
            
            <?php if (isset($variables['sidora_exhibition']['logo']) || $site_name || $site_slogan): ?>
            <div id="header-site-info" class="header-site-info block">
              <div id="header-site-info-inner" class="header-site-info-inner inner">
                <?php if (!empty($variables['sidora_exhibition']['logo'])) : ?>
                <div id="logo">
                  <img src="<?php print $variables['sidora_exhibition']['logo']; ?>" alt="<?php print t('Exhibition Logo'); ?>" <?php print (isset($variables['sidora_exhibition']['logo_height']) ? 'height="'.$variables['sidora_exhibition']['logo_height'].'"' : ''); ?> <?php print (isset($variables['sidora_exhibition']['logo_width']) ? 'width="'.$variables['sidora_exhibition']['logo_width'].'"' : ''); ?> />
                </div>
                <?php else: ?>
                  <?php if (!empty($logo)) : ?>
                <div id="logo">
                  <a href="<?php print check_url($front_page); ?>" title="<?php print t('Home'); ?>"><img src="<?php print $logo; ?>" alt="<?php print t('Home'); ?>"></a>
                </div>
                  <?php endif; ?>
                <?php endif; ?>
                <?php if ($site_name || $site_slogan): ?>
                <div id="site-name-wrapper" class="clearfix">
                  <?php if ($site_name): ?>
                  <span id="site-name"><?php print $site_name; ?></span>
                  <?php endif; ?>
                  <?php if ($site_slogan): ?>
                  <span id="slogan"><?php print $site_slogan; ?></span>
                  <?php endif; ?>
                </div><!-- /site-name-wrapper -->
                <?php endif; ?>
                <?php if ($mission): ?>
                <div id="mission">
                  <p><?php print $mission; ?></p>
                </div>
                <?php endif; ?>
              </div><!-- /header-site-info-inner -->
            </div><!-- /header-site-info -->
            <?php endif; ?>

            <?php echo render($variables['page']['header']); ?>
            
            <?php
            if(!empty($variables['sidora_exhibition']['links'])){
              if($variables['primary_links_tree'] != '' && $variables['sidora_exhibition']['links']) {
                $variables['primary_links_tree'] = str_replace(' last leaf"><a','"><a',$variables['primary_links_tree']);
                $toInsert = '';
            
                foreach($variables['sidora_exhibition']['links'] as $exhibition_link){
                  $toInsert .= '<li class="leaf"><a href="'.$exhibition_link['link'].'">'.$exhibition_link['name'].'</a></li>';
                }
                $variables['primary_links_tree'] = str_replace('</ul>',$toInsert.'</ul>',$variables['primary_links_tree']);
              }
            }
            ?>
            
            <?php 
              if (!empty($variables['primary_links_tree'])) print theme('grid_block', array('content' => $variables['primary_links_tree'], 'id' => 'primary-menu')); ?>
            <?php print theme('grid_block', array('content' => $breadcrumb, 'id' => 'breadcrumbs')); ?>

          </div><!-- /header-group-inner -->
        </div><!-- /header-group -->
      </div><!-- /header-group-wrapper -->

      
      <!-- preface-top row: width = grid_width -->
      <?php //print theme('grid_row', $preface_top, 'preface-top', 'full-width', $grid_width); ?>
      <?php print theme('grid_block', array('content' => render($page['preface_top']), 'id' => 'preface-top')); ?>

      <!-- main row: width = grid_width -->
      <div id="main-wrapper" class="main-wrapper full-width">
        <div id="main" class="main row <?php print $grid_width; ?>">
          <div id="main-inner" class="main-inner inner clearfix">
            <?php //print theme('grid_row', $sidebar_first, 'sidebar-first', 'nested', $sidebar_first_width); ?>
            <?php print theme('grid_block', array('content' => render($page['sidebar_first']), 'id' => 'sidebar-first-outer')); ?>
            <script>
              $(function(){
                $('#sidebar-first-outer').addClass('grid16-3');
                $('#sidebar-first-inner').addClass('grid16-16');
                $('#sidebar-first').addClass('grid16-16');
                $('#sidebar-first').removeClass('grid16-3');
                $('#primary-menu ul').addClass('sf-menu sf-js-enabled');
              });
            </script>

            <!-- main group: width = grid_width - sidebar_first_width -->
            <div id="main-group" class="main-group row nested <?php print $main_group_width; ?>">
              <div id="main-group-inner" class="main-group-inner inner">
                <?php //print theme('grid_row', $page['preface_bottom'], 'preface-bottom', 'nested'); ?>
                <?php print theme('grid_block', array('content' => render($page['preface_bottom']), 'id' => 'preface-bottom')); ?>

                <div id="main-content" class="main-content row nested">
                  <div id="main-content-inner" class="main-content-inner inner">
                    <!-- content group: width = grid_width - (sidebar_first_width + sidebar_last_width) -->
                    <div id="content-group" class="content-group row nested <?php print $content_group_width; ?>">
                      <div id="content-group-inner" class="content-group-inner inner">
                        

                        <?php if (isset($page['content_top']) || isset($page['help']) || $messages): ?>
                        <div id="content-top" class="content-top row nested">
                          <div id="content-top-inner" class="content-top-inner inner">
                            <?php if(isset($page['help'])) { ?>
                            <?php print theme('grid_block', array('content' => render($page['help']), 'id' => 'content-help')); ?>
                            <?php } ?>
                            <?php print theme('grid_block', array('content' => $messages, 'id' => 'content-messages')); ?>
                            <?php print render($page['content_top']); ?>
                          </div><!-- /content-top-inner -->
                        </div><!-- /content-top -->
                        <?php endif; ?>

                        <div id="content-region" class="content-region row nested">
                          <div id="content-region-inner" class="content-region-inner inner">
                            <a name="main-content-area" id="main-content-area"></a>
                            <?php print theme('grid_block', array('content' => render($tabs), 'id' => 'content-tabs')); ?>
                            <div id="content-inner" class="content-inner block">
                              <div id="content-inner-inner" class="content-inner-inner inner">
                                <?php if ($title): ?>
                                <h1 class="title"><?php print $title; ?></h1>
                                <?php endif; ?>
                                <?php if ($page['content']): ?>
                                <div id="content-content" class="content-content">
                                  <?php print render($page['content']); ?>
                                  <?php print $feed_icons; ?>
                                </div><!-- /content-content -->
                                <?php endif; ?>
                              </div><!-- /content-inner-inner -->
                            </div><!-- /content-inner -->
                          </div><!-- /content-region-inner -->
                        </div><!-- /content-region -->

                        <?php //print theme('grid_row', $content_bottom, 'content-bottom', 'nested'); ?>
                        <?php print theme('grid_block', array('content' => render($page['content_bottom']), 'id' => 'content-bottom')); ?>
                      </div><!-- /content-group-inner -->
                    </div><!-- /content-group -->
                    
                    <?php
                    print render($page['sidebar_second']);
                    ?>
                    
                    <?php //print theme('grid_row', $sidebar_last, 'sidebar-last', 'nested', $sidebar_last_width); ?>
                    <?php //print theme('grid_block', array('content' => render($page['sidebar_second']), 'id' => 'sidebar-last')); ?>
                  </div><!-- /main-content-inner -->
                </div><!-- /main-content -->

                <?php //print theme('grid_row', $postscript_top, 'postscript-top', 'nested'); ?>
                <?php print theme('grid_block', array('content' => render($page['postscript_top']), 'id' => 'postscript-top')); ?>
              </div><!-- /main-group-inner -->
            </div><!-- /main-group -->
          </div><!-- /main-inner -->
        </div><!-- /main -->
      </div><!-- /main-wrapper -->

      <!-- postscript-bottom row: width = grid_width -->
      <?php //print theme('grid_row', $postscript_bottom, 'postscript-bottom', 'full-width', $grid_width); ?>
      <?php print theme('grid_block', array('content' => render($page['postscript_bottom']), 'id' => 'postscript-bottom')); ?>

      <!-- bottom row: width = grid_width -->
      <?php //print theme('grid_row', $bottom, 'bottom', 'full-width', $grid_width); ?>
      <?php print theme('grid_block', array('content' => render($page['bottom']), 'id' => 'bottom')); ?>

      <!-- footer row: width = grid_width -->
      <?php //print theme('grid_row', $footer, 'footer', 'full-width', $grid_width); ?>
      <?php print theme('grid_block', array('content' => drupal_render($page['footer']), 'id' => 'footer')); ?>

      <!-- footer-message row: width = grid_width -->
      <div id="footer-message-wrapper" class="footer-message-wrapper full-width">
        <div id="footer-message" class="footer-message row <?php print $grid_width; ?>">
          <div id="footer-message-inner" class="footer-message-inner inner clearfix">
            <?php print theme('grid_block', array('content' => drupal_render($page['footer_message']), 'id' => 'footer-message-text')); ?>
            <p><a href="<?php print $GLOBALS['base_url'];?>/Terms+of+Use">Terms of Use</a>&nbsp; &nbsp; &nbsp; &nbsp; &nbsp;<a href="http://www.si.edu/privacy/">Privacy Notice</a></p>
          </div><!-- /footer-message-inner -->
        </div><!-- /footer-message -->
      </div><!-- /footer-message-wrapper -->

    </div><!-- /page-inner -->
  </div><!-- /page -->
  <?php //print $closure; ?>


<?php
if(isset($_GET['dump']))
  echo '<pre>' . print_r($variables, true) . '</pre>';
?>
