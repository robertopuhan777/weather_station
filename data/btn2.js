$('btn2').on('click', function() {
    $('meta[name=theme-color]').remove();
    $('head').append( '<meta name="theme-color" content="white">' );
    document.body.style.transition = "background 0.2s,color 0.2s";
    document.body.style.backgroundColor = "#ffffff";
    document.body.style.color = "black";
    document.getElementById("btn2").style.color = "#ED6E4C";
	document.getElementById("btn1").style.color = "darkgrey";
	document.getElementById("Home-open-button-1").style.color = "darkgrey";
	document.getElementById("Weather-open-button-1").style.color = "darkgrey";
	document.getElementById("Sensor-open-button-1").style.color = "darkgrey";
	document.getElementById("Sun-open-button-1").style.color = "darkgrey";
	document.getElementById("Moon-open-button-1").style.color = "darkgrey";
	document.getElementById("Radio-open-button-1").style.color = "darkgrey";
	document.getElementById(btn1).style.display = "none";
});
