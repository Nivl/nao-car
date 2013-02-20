io.injectScripts(['js/countdown.js', 'js/entities.js'],
  function() {
      var hash = window.location.hash;
      var cd;
      if (hash) {
        cd = eval(hash.substr(1));
      } else {
        cd = 20*60;
      }
	  countdownTo(new Date().getTime()+cd*1000, function() {
	  });
	
  });

io.injectScripts(['js/countdown.js', 'js/entities.js'],
  function() {
      var hash = window.location.hash;
	  var cd;
      if (hash) {
        cd = eval(hash.substr(1));
      } else {
        cd = 20*60;
      }
	  countdownTo(new Date().getTime()+cd*1000, function() {
		  entitiesLoader.load('Watch again','','','js/dots.js','js/colors.js');
 
	  });
	
  });
 
