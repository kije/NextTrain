var locationRequest, stationBoardRequest, updateTimeout, errorCount = 0, intervalDelay = 132000  /* 2.2 Minutes */;

var resultLimit = 3, results = [];

function sendResultsToPebble() {
	var i = 0;
	var sendNext = function(e) {
		console.log("Successfully delivered message with transactionId="+e.data.transactionId);
		
		
		if (i > resultLimit){
			results = [];
		} else {
			Pebble.sendAppMessage(results[i], sendNext, function(e) {
				console.log("Unable to deliver message with transactionId="+ e.data.transactionId+ " Error is: " + e.error.message);
			});
		}
		i++;
	};
	Pebble.sendAppMessage(results[i], sendNext, function(e) {
				console.log("Unable to deliver message with transactionId="+ e.data.transactionId+ " Error is: " + e.error.message);
			});
}

function processStationboardRequest(e) {
	console.log("opendata request load....");
	if (stationBoardRequest.readyState == 4 && stationBoardRequest.status == 200) {
		if(stationBoardRequest.status == 200) {
			errorCount = 0;
			var response = JSON.parse(stationBoardRequest.responseText);
			if ('stationboard' in response) {
				var stationboard = response.stationboard;
				for (var s in stationboard) {
					
					results.push({
						"station_name": stationboard[s].stop.station.name,
						 "delay": (stationboard[s].stop.delay? stationboard[s].stop.delay : 0),
						"platform": stationboard[s].stop.platform,
						"to": stationboard[s].to,
						"category": stationboard[s].category,
						"departure_time": (new Date(stationboard[s].stop.departure).getTime())/1000,
						"arival_time": parseInt(0)//stationboard[s]['stop']['departure']	
					});
					
					if (results.length >= resultLimit) {
						break;	
					}
				}
				
				console.log(JSON.stringify(results));
				
			}
			
		} else { 
			console.log("Error");
			errorCount++;
		}
	}
}

function processLocationRequest(e) { // stations 
	console.log("location request load....");
	if (locationRequest.readyState == 4 && locationRequest.status == 200) {
		if(locationRequest.status == 200) {
			errorCount = 0;
			var response = JSON.parse(locationRequest.responseText);
			if (response.stations.length > 0) {
				var stations =  response.stations;
				//var closest_station = stations[0].distance;
				for (var station in stations) {
					if ('distance' in stations[station]) { 
						var location_id = stations[station].id;
						console.log(location_id);						
						var url = "http://transport.opendata.ch/v1/stationboard?"+
									"id="+location_id+
									"&limit="+resultLimit+
									"&fields[]=stationboard/stop/station/name&fields[]=stationboard/stop/departure&fields[]=stationboard/stop/delay&fields[]=stationboard/stop/platform&fields[]=stationboard/to&fields[]=stationboard/category";
						stationBoardRequest.open('GET', url, false);
						console.log("Send Request to OpenData...\nURL: "+url);
						stationBoardRequest.send(null);
						if (results.length >= resultLimit) {
							break;	
						}
					} else {
						return;
					}
				}
				sendResultsToPebble();
			} else {
				console.log('No result\n Response: '+locationRequest.responseText);
			}
		} else { 
			console.log("Error");
			errorCount++;
		}
	}
}



function processPosition(position) {
	console.log("Prepare Request...");
	var url = 'http://transport.opendata.ch/v1/locations?x='+position.coords.latitude+'&y='+position.coords.longitude;
	locationRequest.open('GET', url, false);
	console.log("Send Location Request...\nURL: "+url);
	locationRequest.send(null);
	updateTimeout = setTimeout(updateData, intervalDelay);
}


function updateData() {
	console.log("Update data...");
	navigator.geolocation.getCurrentPosition(processPosition);
}


Pebble.addEventListener("ready", function(e) {
	console.log("JavaScript app ready and running!");
	/*console.warn("TO SAVE ENERGY ON DAYLY USE, REQUESTS ARE CURRENTLY DISABLED");*/
	locationRequest = new XMLHttpRequest();
	stationBoardRequest = new XMLHttpRequest();
	locationRequest.onload = processLocationRequest;
	stationBoardRequest.onload = processStationboardRequest;
	updateData();
});

Pebble.addEventListener("showConfiguration", function(e) {
	Pebble.openURL("http://kije.ch");
});
 

Pebble.addEventListener("webviewclosed", function(e) {
	console.log("Configuration window returned: " + e.response);
});