var req, updateTimeout, errorCount = 0;

function onRequestLoad(e) {
	console.log("Request load....");
	if (req.readyState == 4 && req.status == 200) {
		if(req.status == 200) {
			errorCount = 0;
			var response = JSON.parse(req.responseText);
			console.log(req.responseText);
			var transactionId = Pebble.sendAppMessage( response,
			  function(e) {
				console.log("Successfully delivered message with transactionId="
				  + e.data.transactionId);
			  },
			  function(e) {
				console.log("Unable to deliver message with transactionId="
				  + e.data.transactionId
				  + " Error is: " + e.error.message);
			  }
			);
		} else { 
			console.log("Error");
			errorCount++;
		}
	}
}

function processPosition(position) {
	console.log("Prepare Request...");
	var url = 'http://kije.cetus.uberspace.de/serv/NextTrain/NextTrainServer/?lat='+position.coords.latitude+'&long='+position.coords.longitude;
	req.open('GET', url, true);
	console.log("Send Request...\nURL: "+url);
	req.send(null);
}


function updateData() {
	console.log("Update data...");
	navigator.geolocation.getCurrentPosition(processPosition);
	updateTimeout = setTimeout(updateData, 120000 /* 2 Minutes */);
}


Pebble.addEventListener("ready", function(e) {
	console.log("JavaScript app ready and running!");
	/*console.warn("TO SAVE ENERGY ON DAYLY USE, REQUESTS ARE CURRENTLY DISABLED");*/
	req = new XMLHttpRequest();
	req.onload = onRequestLoad;
	updateData();
});

Pebble.addEventListener("showConfiguration", function(e) {
	Pebble.openURL("http://kije.ch");
});
 

Pebble.addEventListener("webviewclosed", function(e) {
	console.log("Configuration window returned: " + e.response);
});