
  var Socket = new WebSocket('ws://' + window.location.hostname + ':81/');

//  var connectionStatus = document.getElementById("status");
  Socket.onopen = function(event){
    console.log("web socket opened");
  //  connectionStatus.innerHTML = "web socket connected";
  };

  Socket.onerror = function(event){
    console.log("web socket error");
//    connectionStatus.innerHTML = "Oop...got error!!!";
  };

  Socket.onclose = function(event){
    console.log("web socket closed");
//    connectionStatus.innerHTML = "web socket disconnected";
  };

  Socket.onmessage = function(event){
    console.log("web socket connect. It transfer below message");
    console.log(event);
  };


  lastSend = 0;
  var FandBSliderElement = document.getElementById("ForwardAndBackwardRange");
  var FandBSliderOutput = document.getElementById("ValueOfForwardAndBackward");
  function sendFandBDirection(FandBSliderElement){
    var now = (new Date).getTime();
    if(lastSend > now -20)return;
    lastSend = now;

    console.log(FandBSliderElement);
  /*  if(FandBSliderElement>31){
      FandBSliderOutput.innerHTML = "go backward";
    }else if(FandBSliderElement<29){
      FandBSliderOutput.innerHTML = "go forward";
    }else{
      FandBSliderOutput.innerHTML = "stop";
    }*/

    Socket.send("F" +FandBSliderElement);
  }



  var LeftandRightSliderElement = document.getElementById("LeftAndRightRange");
  var LandRSliderOutput = document.getElementById("ValueOfLeftAndRight");
  function sendLeftAndRight(LeftandRightSliderElement){
    var now = (new Date).getTime();
    if(lastSend > now -20)return;
    lastSend = now;
    console.log(LeftandRightSliderElement);
  /*  if(LeftandRightSliderElement>30){
      LandRSliderOutput.innerHTML = "turn right";
    }else if(LeftandRightSliderElement<30){
      LandRSliderOutput.innerHTML = "turn left";
    }
    */

    Socket.send("L" +LeftandRightSliderElement);

  }
