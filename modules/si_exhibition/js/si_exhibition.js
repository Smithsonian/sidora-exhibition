function si_exhibition_ds(pid, dsid, title) {
  (function ($) {
    var atitle = title || '';
    /*
    //If you wanted to show in iframe
        Shadowbox.open({
          content:    "<iframe src='"+Drupal.settings.basePath + "si/exhibition/datastream?" + "pid="+pid+"&dsid="+dsid+"' style='width:98%;height:98%;' frameborder=0></iframe>",
          player:     "html",
          type:       "iframe",
          title:      decodeURIComponent((atitle).replace(/\+/g, '%20')),
          gallery:    pid,
          height:     600,
          width:      960//width + 15 // scroll bar
        });
    return false; // Prevent Event Propagation
    */
    $.ajax({
      type: "GET",
      url: Drupal.settings.basePath + "si/exhibition/datastream",
      cache: false,
      data: {
        pid: pid,
        dsid: dsid
      },
      dataType: 'json'
    })
    .done(
      function(data) {
        var width = 960;
        if (dsid == 'CSV') {
          var widthType = 'auto';
        }
        else {
          var widthType = width;
        }
        // @todo Do we actually need this?
        // Evil height check hack.
        var check = $('<div id="height-check"></div>').appendTo('body').html(data.output).css({'float': 'left', 'width': widthType, 'display': 'none'});
        var checkHeight = check.outerHeight(true);
        var checkWidth = check.outerWidth(true);
        // If the rendered width is bigger than our default let it go wider.
        if (checkWidth > width) {
          width = checkWidth;
        }
        var height = checkHeight+11;
        var windowHeight = $(window).height(true);
        if (checkHeight > windowHeight) {
          var height = windowHeight - 100;
        }
        // dump evidence of evil height check hack. (this hack never happend)
        $('#height-check').remove();
        Shadowbox.open({
          content:    data.output,
          player:     "html",
          title:      decodeURIComponent((atitle).replace(/\+/g, '%20')),
          gallery:    pid,
          height:     height,
          width:      width + 15, // scroll bar
          options:    {onFinish: function(){recolorShadowboxTable();}}
        });
      }
    );
    return false; // Prevent Event Propagation
  })(jQuery);
}
function recolorShadowboxTable(){
/*
  //if your table came without even and odd numbering
  var toAddTo = $('#sb-player .metadata-table tr');
  for (i = 0; i < toAddTo.length; i++){
    if (i % 2 == 0){
      $(toAddTo[i]).addClass("odd");
    }else{
      $(toAddTo[i]).addClass("even");
    }
  }
*/
}

function si_exhibition_flexpaper(pid, title) {
  title = title || '';
  Shadowbox.open({
    content:    Drupal.settings.basePath + "si/exhibition/flexpaper/" + pid,
    player:     "iframe",
    title:      decodeURIComponent((title).replace(/\+/g, '%20')),
    gallery:    pid
  });
  return false;
}

(function ($) {
  //$("#edit-submit").hide();
  $("#edit-show").change(function(){resourceChange(this)});
  $("#edit-limit").change(function(){resourceChange(this)});
})(jQuery);

function resourceChange(item){
  (function ($) {
    $(item).closest("form").submit();
  })(jQuery);
}

(function($){
  $(document).ready(function() {
 $('#forjstree').bind('loaded.jstree', function(e,data){setTimeout(function(){
   jstreeIdSelected =  $('a[href=\"'+window.location.pathname + window.location.search+'\"]').parent().attr('id');
   toOpen = [];
   toOpen.unshift(jstreeIdSelected);
   while(toOpen[0] != false){
     toOpen.unshift($('#forjstree').jstree('get_parent','#'+toOpen[0]));
   }
   $('#forjstree').jstree('close_all');
   for (i = 0; i < toOpen.length; i++){
     if (toOpen[i]) $('#forjstree').jstree('open_node','#'+toOpen[i]);
   }
   $('#forjstree').jstree('select_node','a[href=\"'+window.location.pathname + window.location.search+'\"]');
   $('#forjstree').bind('select_node.jstree', function(e,data) {
     window.location = jQuery('#'+data.selected[0]).children('a').attr('href');
   });
   $('#forjstree').show();
 },1000)});
 $('#forjstree').hide();
 $('#forjstree').jstree({
   //default config
 });
 $('#forjstree').jstree('open_all');
 });
})(jQuery);