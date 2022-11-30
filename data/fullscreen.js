function fullscreen() {
    if ((document.fullScreenElement && document.fullScreenElement !== null) ||
        (!document.mozFullScreen && !document.webkitIsFullScreen)) {
        if (document.documentElement.requestFullScreen) {
            document.documentElement.requestFullScreen();
        } else if (document.documentElement.mozRequestFullScreen) {
            document.documentElement.mozRequestFullScreen();
        } else if (document.documentElement.webkitRequestFullScreen) {
            document.documentElement.webkitRequestFullScreen(Element.ALLOW_KEYBOARD_INPUT);
        }
    } else {
        if (document.cancelFullScreen) {
            document.cancelFullScreen();
      
        } else if (document.mozCancelFullScreen) {
            document.mozCancelFullScreen();
      
        } else if (document.webkitCancelFullScreen) {
            document.webkitCancelFullScreen();
      
        }
    }
}

document.addEventListener('fullscreenchange', FSHandler);
document.addEventListener('webkitfullscreenchange', FSHandler);
document.addEventListener('mozfullscreenchange', FSHandler);
document.addEventListener('MSFullscreenChange', FSHandler);

function FSHandler() {
  // FS = FullScreen
    if (!document.fullscreenElement 
  && !document.webkitIsFullScreen 
  && !document.mozFullScreen 
  && !document.msFullscreenElement) {
  
    document.getElementById('power-switch').checked = false;
    
    } else {
    
    document.getElementById('power-switch').checked = true;
  }
}

function view_fullscreen(button) {
   var x = $('#showhide1');
   $(button).find('i').remove();
   if ($(button).text().trim() == 'Show') {
     $(button).html($('<i/>',{class:'fa fa-eye-slash'})).append(' Hide');
     x.fadeIn();
    }
    else {
      $(button).html($('<i/>',{class:'fa fa-eye'})).append(' Show');
      x.fadeOut();
    }
}