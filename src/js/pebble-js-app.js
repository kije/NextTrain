/**
	JS for NextTrain
	Fetches data from transport.opendata.ch -> Huge thanks to these guys 
	
	TODO:
		- Configuration Window
			- Set favorite connection
		- Clean up the mess in here
		- Optimize the way data is updated on pebble
		- Pebble should be able to request update
*/

var locationRequest, stationBoardRequest, connectionRequest, updateTimeout, errorCount = 0, intervalDelay = 132000  /* 2.2 Minutes */;

var resultLimit = 3, results = [];

function sendResultsToPebble() {
	var i = 0;
	var sendNext = function(e) {
		console.log("Successfully delivered message with transactionId="+e.data.transactionId);
		
		if (i >= resultLimit){
			console.log("Clean results");
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

function processConnectionRequest(e) {
	var result = this;
	console.log("opendata connection request load....");
	if (connectionRequest.readyState == 4 && connectionRequest.status == 200) {
		if(connectionRequest.status == 200) {
			errorCount = 0;
			var response = JSON.parse(connectionRequest.responseText);
			if ('connections' in response) {
				var date = new Date(response.connections[0].to.arrival);
				result.result = (date.getTime()-(date.getTimezoneOffset() * 60000))/1000;
				
			}
			
		} else { 
			console.log("Error");
			errorCount++;
		}
	}
}

/**
 * Make sure that from is id
 * Returns arival time
**/
function fetchArivalTime(from, to) {
	console.log("Fetch connection");
	var url = "http://transport.opendata.ch/v1/connections?"+
		"from="+from+
		"&to="+to+
		"&limit="+1;
	var res = {'result': 0}; // need object for pass-by-reference
	connectionRequest.open('GET', url, false);
	connectionRequest.onload = processConnectionRequest.bind(res);
	console.log("Send Request to OpenData...\nURL: "+url);
	connectionRequest.send(null);
	return res.result;
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
					var date = new Date(stationboard[s].stop.departure);
					console.log("dep: "+stationboard[s].stop.departure+" offset: "+date.getTimezoneOffset() );
					results.push({
						"station_name": stationboard[s].stop.station.name,
						"delay": (stationboard[s].stop.delay? stationboard[s].stop.delay : 0),
						"platform": stationboard[s].stop.platform,
						"to": stationboard[s].to,
						"category": stationboard[s].category,
						"departure_time": (date.getTime()-(date.getTimezoneOffset() * 60000))/1000,
						"arival_time": fetchArivalTime(stationboard[s].stop.station.id, stationboard[s].to)
					});

					
					
					if (results.length >= resultLimit) {
						break;	
					}
				}
				
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
				var closest_station = stations[0];
				var distance_max_treshold = closest_station.distance+(closest_station.distance/100)*3; // + 3%
				console.log("max dist: "+distance_max_treshold);
				var distance_min_treshold = closest_station.distance-(closest_station.distance/100)*3; // - 3%
				console.log("min dist: "+distance_min_treshold);
				var stations_in_range = [];
				var station_with_highest_score = stations[0];
				for (var station in stations) {
					if (stations[station].distance < distance_max_treshold && stations[station].distance > distance_min_treshold) {
						stations_in_range.push(stations[station]);
					}

					if (station_with_highest_score.score < stations[station].score) {
						station_with_highest_score = stations[station];
					}
				}

				stations_in_range[0] = station_with_highest_score;
				if (stations_in_range.length <= 1) {
					console.log("To few results in nearest stations! Fall back to all results")
					stations_in_range = stations;
				}
				var per_station_limit = Math.ceil(resultLimit/Math.min(stations_in_range.length, 4));
				console.log("Per station limit: "+per_station_limit)
				for (var station in stations_in_range) {
					
						var location_id = stations_in_range[station].id;
						console.log(location_id);						
						var url = "http://transport.opendata.ch/v1/stationboard?"+
									"id="+location_id+
									"&limit="+per_station_limit+
									"&fields[]=stationboard/stop/station/name&fields[]=stationboard/stop/station/id&fields[]=stationboard/stop/departure&fields[]=stationboard/stop/delay&fields[]=stationboard/stop/platform&fields[]=stationboard/to&fields[]=stationboard/category";
						stationBoardRequest.open('GET', url, false);
						console.log("Send Request to OpenData...\nURL: "+url);
						stationBoardRequest.send(null);
						if (results.length >= resultLimit) {
							break;	
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
	locationRequest = new XMLHttpRequest();
	stationBoardRequest = new XMLHttpRequest();
	connectionRequest = new XMLHttpRequest();
	locationRequest.onload = processLocationRequest;
	stationBoardRequest.onload = processStationboardRequest;
	connectionRequest.onload = processConnectionRequest;
	updateTimeout = setTimeout(updateData, 1000); // -> to show splash-screen a little bit longer ;)
});

Pebble.addEventListener("showConfiguration", function(e) {
	Pebble.openURL("http://kije.ch");
});
 

Pebble.addEventListener("webviewclosed", function(e) {
	console.log("Configuration window returned: " + e.response);
});