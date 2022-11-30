window.addEventListener('load', getOWMReadings);

// Create Temperature Gauge
var gaugeTemp = new RadialGauge({
  renderTo: 'gauge-temperature',
  width: 275,
  height: 275,
  units: "Wind \u00B0",
  minValue: 0,
  startAngle: 0, 
  ticksAngle: 360,
  maxValue: 360,
  colorValueBoxRect: "transparent",
  colorValueBoxRectEnd: "transparent",
  colorValueBoxBackground: "transparent",
  valueDec: 0,
  valueInt: 2,
  majorTicks: [
      "S",
	  "SSW",
	  "SW",
	  "WSW",
      "W",
	  "WNW",
	  "NW",
	  "NNW",
	  "N",
	  "NNE",
	  "NE",
	  "ENE",
	  "E",
	  "ESE",
	  "SE",
	  "SSE",
	  "S"
  ],
  minorTicks: 22.5,
  strokeTicks: true,
  highlights: [
      {
          "from": 7,
          "to": 10,
          "color": "rgba(255, 0, 0, 0)"
      }
  ],
  colorPlate: "transparent",
  colorBarProgress: "#EC9DA4",
  colorBarProgressEnd: "#CC2936",
  borderShadowWidth: 0,
  borders: false,
  needleType: "arrow",
  needleWidth: 1,
  needleCircleSize: 7,
  needleCircleOuter: true,
  needleCircleInner: false,
  animationDuration: 500,
  animationRule: "linear",
  barWidth: 10,
}).draw();

// Function to get current readings on the webpage when it loads for the first time
function getOWMReadings(){
  var xhr = new XMLHttpRequest();
  xhr.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      var myObj = JSON.parse(this.responseText);
      console.log(myObj);
      var temp = myObj.Wind_Direction;
      gaugeTemp.value = temp;
    }
  }; 
  xhr.open("GET", "/owmreadings", true);
  xhr.send();
}

if (!!window.EventSource) {
  var source = new EventSource('/events');
  
  source.addEventListener('open', function(e) {
    console.log("Events Connected");
  }, false);

  source.addEventListener('error', function(e) {
    if (e.target.readyState != EventSource.OPEN) {
      console.log("Events Disconnected");
    }
  }, false);
  
  source.addEventListener('message', function(e) {
    console.log("message", e.data);
  }, false);
  
  source.addEventListener('OWM_readings', function(e) {
    console.log("OWM_readings", e.data);
    var myObj = JSON.parse(e.data);
    console.log(myObj);
    gaugeTemp.value = myObj.Wind_Direction;
  }, false);
}