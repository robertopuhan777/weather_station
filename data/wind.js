window.addEventListener('load', getOWMReadings);

// Create Temperature Gauge
var gaugeTemp = new RadialGauge({
  renderTo: 'gauge-temperature',
  width: 255,
  height: 255,
  units: "Wind Speed m/s",
  minValue: 0,
  startAngle: 90, 
  ticksAngle: 180,
  maxValue: 10,
  colorValueBoxRect: "transparent",
  colorValueBoxRectEnd: "transparent",
  colorValueBoxBackground: "transparent",
  valueDec: 2,
  valueInt: 2,
  majorTicks: [
      "0",
	  "1",
      "2",
	  "3",
	  "4",
	  "5",
	  "6",
	  "7",
	  "8",
	  "9",
      "10"
  ],
  minorTicks: 10,
  strokeTicks: true,
  highlights: [
      {
          "from": 7,
          "to": 10,
          "color": "rgba(200, 50, 50, .75)"
      }
  ],
  colorPlate: "transparent",
  colorBarProgress: "#EC9DA4",
  colorBarProgressEnd: "#CC2936",
  borderShadowWidth: 0,
  borders: false,
  needleType: "arrow",
  needleWidth: 2,
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
      var temp = myObj.Wind_Speed;
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
    gaugeTemp.value = myObj.Wind_Speed;
  }, false);
}