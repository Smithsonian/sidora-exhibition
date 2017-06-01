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
    jQuery("#exhibition_gray_overlay").remove();
    jQuery("body").append("<div id='exhibition_gray_overlay' style='background-color:rgba(0,0,0,1);opacity:0.5;width:100%;height:100%;position:absolute;top:0;left:0'>");
    jQuery("#exhibition_gray_overlay").append('<div id="sb-loading"><div id="sb-loading-inner"><span>&nbsp;</span></div></div>');
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
    ).always(function(jqXHR, textStatus) {
      jQuery("#exhibition_gray_overlay").fadeOut();
    });
  })(jQuery);
  return false; // Prevent Event Propagation
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
  var newLocation = window.location;
  if (window.location.search.indexOf("?") == -1) {
    newLocation += "?";
  }
  if (window.location.search.indexOf("&time=") == -1) {
    newLocation += "&time=" + (new Date).getTime();
    window.location = newLocation;
    return;
  }
 $('#forjstree').bind('loaded.jstree', function(e,data){setTimeout(function(){

   var params = getSearchParameters();
   var idForSelection = 'a[href=\"'+window.location.pathname;
   if (typeof(params['path']) !== "undefined") {
     idForSelection += "?path=" + params['path'];
   }
   idForSelection += '\"]';

   jstreeIdSelected =  $(idForSelection).parent().attr('id');
   toOpen = [];
   toOpen.unshift(jstreeIdSelected);
   while(toOpen[0] != false){
     toOpen.unshift($('#forjstree').jstree('get_parent','#'+toOpen[0]));
   }
   $('#forjstree').jstree('close_all');
   for (i = 0; i < toOpen.length; i++){
     if (toOpen[i]) $('#forjstree').jstree('open_node','#'+toOpen[i]);
   }
   $('#forjstree').jstree('select_node',idForSelection);
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


function getSearchParameters() {
      var prmstr = window.location.search.substr(1);
      return prmstr != null && prmstr != "" ? transformToAssocArray(prmstr) : {};
}

function transformToAssocArray( prmstr ) {
    var params = {};
    var prmarr = prmstr.split("&");
    for ( var i = 0; i < prmarr.length; i++) {
        var tmparr = prmarr[i].split("=");
        params[tmparr[0]] = tmparr[1];
    }
    return params;
}

