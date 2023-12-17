var gateway = `ws://${window.location.hostname}/ws`;
var websocket;

//data format
var DTSINGLE = "S"
var DTMULTI  = "M"
var DTTEXT   = "T"

//message type
var TVALUE   = "V";
var TREQUEST = "R";

//object type
var TANALOG  = "A";
var TSLIDER  = "B";
var TCOLOR   = "C";
var TSWITCH  = "D";
var TTEXT    = "E";
var TOFFSET  = "F";
var TGraph   = "G";
var TPIC     = "H";
var TINDEX   = "I";

window.addEventListener('load', onload);

/*
  // Create Temperature Gauge
  var gaugeTemp = new LinearGauge({
    renderTo: 'gauge-temperature',
    width: 120,
    height: 400,
    units: "Temperature C",
    minValue: 0,
    startAngle: 90,
    ticksAngle: 180,
    maxValue: 40,
    colorValueBoxRect: "#049faa",
    colorValueBoxRectEnd: "#049faa",
    colorValueBoxBackground: "#f1fbfc",
    valueDec: 2,
    valueInt: 2,
    majorTicks: [
        "0",
        "5",
        "10",
        "15",
        "20",
        "25",
        "30",
        "35",
        "40"
    ],
    minorTicks: 4,
    strokeTicks: true,
    highlights: [
        {
            "from": 30,
            "to": 40,
            "color": "rgba(200, 50, 50, .75)"
        }
    ],
    colorPlate: "#fff",
    colorBarProgress: "#CC2936",
    colorBarProgressEnd: "#049faa",
    borderShadowWidth: 0,
    borders: false,
    needleType: "arrow",
    needleWidth: 2,
    needleCircleSize: 7,
    needleCircleOuter: true,
    needleCircleInner: false,
    animationDuration: 1500,
    animationRule: "linear",
    barWidth: 10,
  }).draw();

  // Create Humidity Gauge
  var gaugeHum = new RadialGauge({
    renderTo: 'gauge-humidity',
    width: 300,
    height: 300,
    units: "Humidity (%)",
    minValue: 0,
    maxValue: 100,
    colorValueBoxRect: "#049faa",
    colorValueBoxRectEnd: "#049faa",
    colorValueBoxBackground: "#f1fbfc",
    valueInt: 2,
    majorTicks: [
        "0",
        "20",
        "40",
        "60",
        "80",
        "100"

    ],
    minorTicks: 4,
    strokeTicks: true,
    highlights: [
        {
            "from": 80,
            "to": 100,
            "color": "#03C0C1"
        }
    ],
    colorPlate: "#fff",
    borderShadowWidth: 0,
    borders: false,
    needleType: "line",
    colorNeedle: "#007F80",
    colorNeedleEnd: "#007F80",
    needleWidth: 2,
    needleCircleSize: 3,
    colorNeedleCircleOuter: "#007F80",
    needleCircleOuter: true,
    needleCircleInner: false,
    animationDuration: 1500,
    animationRule: "linear"
  }).draw();
  */

function onload(event)
  {
    initWebSocket();
    setDefaultValue();
    getCurrentValues();
  }

function initWebSocket()
  {
    console.log('Trying to open a WebSocket connection…');
    websocket = new WebSocket(gateway);
    websocket.onopen = onOpen;
    websocket.onclose = onClose;
    websocket.onmessage = onMessage;
  }

function onOpen(event)
  {
    console.log('Connection opened');
  }

function onClose(event)
  {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
  }

function onMessage(event)
  {
    console.log('Message received ', event.data);
    //console.log(event.data);
    var s    = event.data;
    var type = s.substr(2,1);
    var idx  = s.substr(3,1);
    var val  = s.substr(4);
    if (s.substr(0,1) == "S")   // MD_SINGLE
      {
        if (s.substr(1,1) == "V")
          {
            if (type == TANALOG)
              {
                var id = "valANAL_" + (idx);
                //console.log(id);
                document.getElementById(id).innerHTML = val;
              }
            else if (type == TSLIDER)
              {
                var id = "LEDV_" + (idx);
                document.getElementById(id).value = val;
                document.getElementById("val"+ id).innerHTML = val;
              }
            else if (type == TCOLOR)
              {
                var id = "LEDC_" + (idx);
                val = '#' + val;
                document.getElementById(id).value = val;
                //var element = document.getElementById(id);
                //console.log('val = ', val, ' element = ', element.id, ' value = ', element.value);
                //element.value = val;
              }
          }
      }
    //s = 'S' + TVALUE + TSWITCH + s + val.substring(1);
  }

function setDefaultValue()
  {
    document.getElementById("LEDC_1").value = 0;
    document.getElementById("lLEDV_1").innerHTML = '_RGBLED';
    document.getElementById("valLEDV_1").innerHTML = 0;
    document.getElementById("LEDV_1").value = '?';
    document.getElementById("LEDC_2").value = 0;
    document.getElementById("valLEDV_2").innerHTML = 0;
    document.getElementById("LEDV_2").value = '?';
    document.getElementById("LEDC_3").value = 0;
    document.getElementById("valLEDV_3").innerHTML = 0;
    document.getElementById("LEDV_3").value = '?';
    document.getElementById("LEDC_4").value = 0;
    document.getElementById("valLEDV_4").innerHTML = 0;
    document.getElementById("LEDV_4").value = '?';
    document.getElementById("valANAL_0").innerHTML = '?';
    document.getElementById("lANAL_0").innerHTML = 'Temperatur = ';
    document.getElementById("valANAL_1").innerHTML = '?';
    document.getElementById("lANAL_1").innerHTML = 'rel. Feuchte = ';
    //document.getElementById("lANAL_1").innerHTML = 'rel. Feuchte = ';
    document.getElementById("valANAL_2").innerHTML = '?';
    document.getElementById("lANAL_2").innerHTML = 'Luftdruck = ';
    document.getElementById("valANAL_3").innerHTML = '?';
    document.getElementById("lANAL_3").innerHTML = 'Helligkeit = ';
  }

function getCurrentValues()
  {
    var s;
    /*
        var xhr = new XMLHttpRequest();
        xhr.onreadystatechange = function()
          {
            if (this.readyState == 4 && this.status == 200)
              {
                document.getElementById("pwmSlider").value = this.responseText;
                document.getElementById("textSliderValue").innerHTML = this.responseText;
              }
          };
        xhr.open("GET", "/currentValue", true);
        xhr.send();
      */
  }

function updateVal(element)
  {
    var sval, hval;
    var s  = element.id;
    var id = s.substr(0,4);
    var n  = s.substr(s.length - 1, 1);

    if (id == "LEDV")
      {
        sval = document.getElementById(element.id).value;
        //hval = (sval).toString(16);
        document.getElementById("val"+element.id).innerHTML = sval;
        s = s.substr(s.length - 1, 1);
        s = DTSINGLE + TVALUE + TSLIDER + n + sval;
      }

    if (id == "SLSW")
      {
        sval = document.getElementById(element.id).checked;
        s = s.substr(s.length - 1, 1);
        s = DTSINGLE + TVALUE + TSWITCH + s;
        if (sval) s = s + '1';
        else      s = s + '0';
      }

    if (id == "BUTT")
      {
        sval = document.getElementById(element.id).value;
        s = s.substr(s.length - 1, 1);
        s = DTSINGLE + TVALUE + TSWITCH + s + sval.substring(1);
      }

    if (id == "LEDC")
      {
        sval = document.getElementById(element.id).value;
              console.log(element.id + " val " + sval);
        s = s.substr(s.length - 1, 1);
        s = DTSINGLE + TVALUE + TCOLOR + s + sval.substring(1);
      }

    console.log(element.id + "->" + s);
    websocket.send(s);
  }

function handleColor()
  {
    var val = document.getElementById('BUTT_1').value;
    //document.getElementById("but"+element.id).innerHTML = val.substring(1);
    //console.log(val.substring(1));
    webSocket.send(val.substring(1));
  }

// Function to get current readings on the webpage when it loads for the first time
/*
  function getReadings(){
    var xhr = new XMLHttpRequest();
    xhr.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
        var myObj = JSON.parse(this.responseText);
        console.log(myObj);
        var temp = myObj.temperature;
        var hum = myObj.humidity;
        gaugeTemp.value = temp;
        gaugeHum.value = hum;
      }
    };
    xhr.open("GET", "/readings", true);
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

    source.addEventListener('new_readings', function(e) {
      console.log("new_readings", e.data);
      var myObj = JSON.parse(e.data);
      console.log(myObj);
      gaugeTemp.value = myObj.temperature;
      gaugeHum.value = myObj.humidity;
    }, false);
  }

  document.addEventListener('DOMContentLoaded',function()
    {
      function b(B)
        {
          let C;
          switch(B.type)
            {
              case'checkbox':C=B.checked?1:0;break;
              case'range':
              case'select-one':C=B.value;break;
              case'button':
              case'submit':C='1';break;
              default:return;
            }
          const D=`${c}/control?var=${B.id}&val=${C}`;
          fetch(D).then(E=>
            {
              console.log(`request to ${D} finished, status: ${E.status}`)
            })
        }
      var c=document.location.origin;
      const e=B=>{B.classList.add('hidden')},
            f=B=>{B.classList.remove('hidden')},
            g=B=>{B.classList.add('disabled'),
            B.disabled=!0},
            h=B=>{B.classList.remove('disabled'),
            B.disabled=!1},
            i=(B,C,D)=>
              {
                D=!(null!=D)||D;
                let E;
                'checkbox'===B.type?
                    (E=B.checked,C=!!C,B.checked=C)
                    :(E=B.value,B.value=C),
                D&&E!==C?
                    b(B)
                    :!D&&('aec'===B.id?
                        C?e(v):f(v)
                        :'agc'===B.id?
                            C?
                                (f(t),e(s))
                               :(e(t),f(s))
                            :'awb_gain'===B.id?
                                C?
                                    f(x):e(x)
                                    :'face_recognize'===B.id&&(C?h(n):g(n)))
              };
      document.querySelectorAll('.close')
        .forEach(B=>
          {
            B.onclick=()=>{e(B.parentNode)}
          }),
        fetch(`${c}/status`)
          .then(function(B)
            {
              return B.json()
            })
          .then(function(B)
            {
              document.querySelectorAll('.default-action')
                .forEach(C=>
                  {
                    i(C,B[C.id],!1)
                  })
            });
      const j=document.getElementById('stream'),
            k=document.getElementById('stream-container'),
            l=document.getElementById('get-still'),
            m=document.getElementById('toggle-stream'),
            n=document.getElementById('face_enroll'),
            o=document.getElementById('close-stream'),
            p=()=>
              {
                window.stop(),
                m.innerHTML='Start Stream'
              },
            q=()=>
              {
                j.src=`${c+':81'}/stream`,
                f(k),m.innerHTML='Stop Stream'
              };
      l.onclick=()=>
        {
          p(),
          j.src=`${c}/capture?_cb=${Date.now()}`,
          f(k)
        },
      o.onclick=()=>
        {
          p(),e(k)
        },
      m.onclick=()=>
        {
          const B='Stop Stream'===m.innerHTML;
          B?p():q()
        },
      n.onclick=()=>
        {
          b(n)
        },
      document.querySelectorAll('.default-action')
        .forEach(B=>
          {
            B.onchange=()=>b(B)
          });
    const r=document.getElementById('agc'),
          s=document.getElementById('agc_gain-group'),
          t=document.getElementById('gainceiling-group');
    r.onchange=()=>
      {
        b(r),
        r.checked?
            (f(t),e(s))
            :(e(t),f(s))
      };
    const u=document.getElementById('aec'),
          v=document.getElementById('aec_value-group');
    u.onchange=()=>
      {
        b(u),
        u.checked?
            e(v)
            :f(v)
      };
    const w=document.getElementById('awb_gain'),
          x=document.getElementById('wb_mode-group');
    w.onchange=()=>
      {
        b(w),
        w.checked?
            f(x)
            :e(x)
      };
    const y=document.getElementById('face_detect'),
          z=document.getElementById('face_recognize'),
          A=document.getElementById('framesize');
    A.onchange=()=>
      {
        b(A),
        5<A.value&&(i(y,!1),i(z,!1))
      },
    y.onchange=()=>
      {
        return 5<A.value?
                  ( alert('Please select CIF or lower resolution before enabling this feature!'),void i(y,!1)):void(b(y),!y.checked&&(g(n),i(z,!1)))},z.onchange=()=>{return 5<A.value?(alert('Please select CIF or lower resolution before enabling this feature!'),
                    void i(z,!1))
                  :void(b(z),
                        z.checked?
                            (h(n),i(y,!0))
                            :g(n))
      }
    });
  */