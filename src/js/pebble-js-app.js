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


Date.prototype.setISO8601 = function (string) {
    var regexp = "([0-9]{4})(-([0-9]{2})(-([0-9]{2})" +
        "(T([0-9]{2}):([0-9]{2})(:([0-9]{2})(\.([0-9]+))?)?" +
        "(Z|(([-+])([0-9]{2}):([0-9]{2})))?)?)?)?";
    var d = string.match(new RegExp(regexp));

    var offset = 0;
    var date = new Date(d[1], 0, 1);

    if (d[3]) { date.setMonth(d[3] - 1); }
    if (d[5]) { date.setDate(d[5]); }
    if (d[7]) { date.setHours(d[7]); }
    if (d[8]) { date.setMinutes(d[8]); }
    if (d[10]) { date.setSeconds(d[10]); }
    if (d[12]) { date.setMilliseconds(Number("0." + d[12]) * 1000); }
    if (d[14]) {
        offset = (Number(d[16]) * 60) + Number(d[17]);
        offset *= ((d[15] == '-') ? 1 : -1);
    }

    offset -= date.getTimezoneOffset();
    time = (Number(date) + (offset * 60 * 1000));
    this.setTime(Number(time));
}





var locationRequest, stationBoardRequest, connectionRequest, updateTimeout, errorCount = 0, intervalDelay = 132000  /* 2.2 Minutes */;
var per_station_limit = 4;

var resultLimit = 3, results = [];


function sortResults() {
	console.log("Sort array");
	results.sort(function (a,b) {
		if (a.departure_time < b.departure_time) {
			return -1;		
		} else if (a.departure_time == b.departure_time) {
			return 0;					
		}			
		
		return 1;
	});								
}
function sendResultsToPebble() {
	sortResults();
	console.log(JSON.stringify(results, null, '\t'));					
	var sendNext = function(e) {
		if (typeof e != "undefined") {
			console.log("Successfully delivered message with transactionId="+e.data.transactionId);					
		}
		
		if (results.length > 0) {
			Pebble.sendAppMessage(results.shift(), sendNext, function(e) {
				console.log("Unable to deliver message "+ JSON.stringify(e, null, '\t'));
			});
		}
	};

	sendNext();
}

function processConnectionRequest(e) {
	var result = this;
	console.log("opendata connection request load....");
	if (connectionRequest.readyState == 4 && connectionRequest.status == 200) {
		if(connectionRequest.status == 200) {
			errorCount = 0;
			var response = JSON.parse(connectionRequest.responseText);
			if ('connections' in response) {
				var date = new Date();
				date.setISO8601(response.connections[0].to.arrival);
				result.result = parseInt(date.getTime()/1000);
				
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
		"from="+encodeURIComponent(from)+
		"&to="+encodeURIComponent(to)+
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
				var i = 0;

				// filter allready existing connections
				var numbers = [];
				var filtered_stationboard = stationboard.filter(function (element) {
					for (var r in results) {
							   console.log('LLA');
							   if (results[r].id == element.number) {
								   console.log(element.number+" allready exists in results");
								   return false;
							   }
					}
					if (numbers.indexOf(element.number) > -1) {
						console.log(element.number+" allready exists");
						return false;
					}
					numbers.push(element.number);
					return true;
				});
				
				for (var s in filtered_stationboard) {
					if (i > per_station_limit && results.length > resultLimit) {
						break;
					}
					
					console.log(filtered_stationboard[s].stop.departure);	
					var date = new Date();
					date.setISO8601(filtered_stationboard[s].stop.departure);
					console.log(date.toDateString());		
					
					console.log("dep: "+filtered_stationboard[s].stop.departure+" offset: "+date.getTimezoneOffset() );
					results.push({
						"id": filtered_stationboard[s].number,
						"message_type": 0,
						"station_name": filtered_stationboard[s].stop.station.name,
						"delay": filtered_stationboard[s].stop.delay ? filtered_stationboard[s].stop.delay : 0,
						"platform": filtered_stationboard[s].stop.platform,
						"to": filtered_stationboard[s].to,
						"category": filtered_stationboard[s].category,
						"departure_time": parseInt(date.getTime()/1000),
						"arival_time": fetchArivalTime(filtered_stationboard[s].stop.station.id, filtered_stationboard[s].to)
					});

					
					i++;
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
				/*var closest_station = stations[0];
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
				if (stations_in_range.length <= 4) {
					console.log("To few results in nearest stations! Fall back to all results");
					stations_in_range = stations;
				}*/
				per_station_limit = Math.ceil(resultLimit/Math.min(stations.length, 4));
				console.log("Per station limit: "+per_station_limit);
				for (var _station in stations) {
					
						var location_id = stations[_station].id;
						console.log(location_id);						
						var url = "http://transport.opendata.ch/v1/stationboard?"+
									"id="+encodeURIComponent(location_id)+
									"&limit=8"+
									"&fields[]=stationboard/number&fields[]=stationboard/stop/station/name&fields[]=stationboard/stop/station/id&fields[]=stationboard/stop/departure&fields[]=stationboard/stop/delay&fields[]=stationboard/stop/platform&fields[]=stationboard/to&fields[]=stationboard/category";
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
	var url = 'http://transport.opendata.ch/v1/locations?x='+encodeURIComponent(position.coords.latitude)+'&y='+encodeURIComponent(position.coords.longitude)+"&limit=11";
	locationRequest.open('GET', url, false);
	console.log("Send Location Request...\nURL: "+url);
	locationRequest.send(null);
	updateTimeout = setTimeout(updateData, intervalDelay);
}


function updateData() {
	console.log("Update data...");
	// TODO timeout
	navigator.geolocation.getCurrentPosition(processPosition, function (e) {
		console.log("FAIL REQUESTING LOCATION");
	});
	
}


Pebble.addEventListener("ready", function(e) {
	console.log("JavaScript app ready and running!");
	locationRequest = new XMLHttpRequest();
	stationBoardRequest = new XMLHttpRequest();
	connectionRequest = new XMLHttpRequest();
	locationRequest.onload = processLocationRequest;
	stationBoardRequest.onload = processStationboardRequest;
	connectionRequest.onload = processConnectionRequest;
	updateTimeout = setTimeout(updateData, 800); // -> to show splash-screen a little bit longer ;)
});

/*Pebble.addEventListener("showConfiguration", function(e) {
	Pebble.openURL("http://kije.ch");
});
 

Pebble.addEventListener("webviewclosed", function(e) {
	console.log("Configuration window returned: " + e.response);
});*/