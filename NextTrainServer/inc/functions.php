<?php

/**
 * Make get request and returns content
 * @param  string   $url    The URL
 * @param  boolean  $cache  if the content should be cached
 * @param  int      $expire how long the cached information should be valid (in Seconds)
 * @return String           The response
 */
function get_url($url, $cache = false, $expire = 432000 /* 5 Days */) {
    $key = sha1($url);
    if (rDB::exists($key)) {
        $response = rDB::retrive($key);
    } else {
        if (function_exists('curl_init')) {
            $ch = curl_init($url);
            curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
            $response = curl_exec($ch);
        } else {
            $response = http_get($url);
        }

        if ($cache) {
            rDB::store($key, $response, $expire);
        }
    }


    return $response;
}

/**
 * Lookup location name from lat and long over Google Geocoder API
 * @param  String $lat
 * @param  String $long
 * @return [String|false] City name on success, false on fail
 */
function get_location_name_from_lat_long($lat, $long)
{
    $params = array(
        'latlng' => $lat.','.$long,
        'sensor' => 'true',
    );
    $url = 'https://maps.googleapis.com/maps/api/geocode/json?'.http_build_query($params);

    if ($response = get_url($url, true)) {
        if ($data = json_decode($response, true)) {
            if ($data['status'] != 'ZERO_RESULTS') {
                $result = $data['results'][0];
                foreach ($result['address_components'] as $index => $value) {
                    if (in_array('locality', $value['types'])) {
                        return $value['short_name'];
                    }
                }
            }
        }
    }

    return false;
}

function get_stationboard($locality, $limit = 3) {
    $params = array(
        'station' => $locality,
        'limit' => $limit,
        'fields' => array(
            'stationboard/stop/station/name',
            'stationboard/stop/departure',
            'stationboard/stop/delay',
            'stationboard/stop/platform',
            'stationboard/to',
            'stationboard/category',
        ),
    );
    $url = 'http://transport.opendata.ch/v1/stationboard?'.http_build_query($params);

    if ($response = get_url($url, true, 60)) {
         if ($data = json_decode($response, true)) {
            if ($data['stationboard']) {
                return $data['stationboard'];
            }
         }
    }

    return false;
}
