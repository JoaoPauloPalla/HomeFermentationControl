// Complete project details:
// https://randomnerdtutorials.com/esp32-plot-readings-charts-multiple/
// https://randomnerdtutorials.com/stepper-motor-esp32-websocket/

// Functions
async function toggleState(state) {
  try {
    const stateValue = state === 1 ? "ON" : "OFF";
    const data = { state: stateValue };

    const request = await fetch("/state", {
      method: "POST",
      headers: {
        "Content-Type": "application/json",
      },
      body: JSON.stringify(data),
    });

    if (request.ok) {
      document.getElementById("state").innerHTML = stateValue;
    } else {
      throw new Error(
        `Request failed with status ${request.status} - ${request.statusText} - ${request.url}`
      );
    }
  } catch (error) {
    console.error(error);
  }
}

async function setScantime(event) {
  try {
    event.preventDefault();
    const data = { scantime: document.getElementById("scanTime").value };
    const request = await fetch("/scantime", {
      method: "POST",
      headers: {
        "Content-Type": "application/json",
      },
      body: JSON.stringify(data),
    });
  } catch (error) {
    console.error(error);
  }
}

async function setSetpoint() {
  try {
    const data = { setpoint: document.getElementById("setPoint").value };
    const request = await fetch("/setpoint", {
      method: "POST",
      headers: {
        "Content-Type": "application/json",
      },
      body: JSON.stringify(data),
    });
  } catch (error) {
    console.error(error);
  }
}

async function setHysteresis() {
  try {
    const data = { hysteresis: document.getElementById("hysteresis").value };
    const request = await fetch("/hysteresis", {
      method: "POST",
      headers: {
        "Content-Type": "application/json",
      },
      body: JSON.stringify(data),
    });
  } catch (error) {
    console.error(error);
  }
}

//Chart
var chartTemperature = Highcharts.chart({
  chart: { renderTo: "chart-temperature" },
  title: { text: undefined },
  series: [
    {
      name: "Ambient Temperature",
      type: "line",
      color: "#101D42",
      marker: {
        symbol: "circle",
        radius: 3,
        fillColor: "#101D42",
      },
    },
    {
      name: "Internal Temperature",
      type: "line",
      color: "#8B2635",
      marker: {
        symbol: "square",
        radius: 3,
        fillColor: "#8B2635",
      },
    },
  ],
  xAxis: {
    type: "datetime",
    labels: {
      format: "{value:%H:%M}",
    },
  },
  yAxis: {
    type: "value",
    title: { text: "Temperature (C)" },
    min: 0,
    max: 40,
  },
  plotOptions: {
    line: {
      animation: false,
      dataLabels: { enabled: true },
    },
  },
  credits: { enabled: false },
});

// Event source server
if (!!window.EventSource) {
  var source = new EventSource("/events");

  source.addEventListener("load", function (e) {
    //Initial variables
    document.getElementById("state").innerHTML = null;
    document.getElementById("temperature").innerHTML = null;
    document.getElementById("maxTemperature").innerHTML = null;
    document.getElementById("minTemperature").innerHTML = null;
    document.getElementById("scantime").innerHTML = null;
  });

  source.addEventListener(
    "open",
    function (e) {
      console.log("Events Connected");
    },
    false
  );

  source.addEventListener(
    "error",
    function (e) {
      if (e.target.readyState != EventSource.OPEN) {
        console.log("Events Disconnected");
      }
    },
    false
  );

  //Forms
  // source.addEventListener(
  //   "scanTimeForm",
  //   function (e) {
  //     e.preventDefault();
  //     setScantime(e.data);
  //   },
  //   false
  // );

  //Medidas
  source.addEventListener(
    "temperatureEvent",
    function (e) {
      var temperature = parseFloat(e.data);
      //Update temperature
      document.getElementById("temperature").innerHTML = temperature;

      //Update chart temperature
      let data = new Date();
      var x = data.setHours(data.getHours() - 3);
      var y = temperature;
      var z = temperature - 5;

      if (chartTemperature.series[0].data.length > 40) {
        chartTemperature.series[0].addPoint([x, y], true, true, true);
        chartTemperature.series[1].addPoint([x, z], true, true, true);
      } else {
        chartTemperature.series[0].addPoint([x, y], true, false, true);
        chartTemperature.series[1].addPoint([x, z], true, false, true);
      }
    },
    false
  );

  source.addEventListener(
    "maxTemperatureEvent",
    function (e) {
      var temperature = parseFloat(e.data);
      document.getElementById("maxTemperature").innerHTML = temperature;
      console.log("maxTemperature: " + temperature);
    },
    false
  );

  source.addEventListener(
    "minTemperatureEvent",
    function (e) {
      var temperature = parseFloat(e.data);
      document.getElementById("minTemperature").innerHTML = temperature;
      console.log("minTemperature: " + temperature);
    },
    false
  );

  source.addEventListener(
    "scantimeEvent",
    function (e) {
      var scantime = parseFloat(e.data);
      document.getElementById("scanTime").innerHTML = scantime;
      console.log("scantime: " + scantime);
    },
    false
  );

  source.addEventListener(
    "setPointEvent",
    function (e) {
      var setpoint = parseFloat(e.data);
      document.getElementById("setPoint").innerHTML = setpoint;
      console.log("setPoint: " + setpoint);
    },
    false
  );

  source.addEventListener(
    "hysteresisEvent",
    function (e) {
      var hysteresis = parseFloat(e.data);
      document.getElementById("hysteresis").innerHTML = hysteresis;
      console.log("hysteresis: " + hysteresis);
    },
    false
  );
}
