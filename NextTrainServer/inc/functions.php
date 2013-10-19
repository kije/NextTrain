<?php

/**
 * Lookup location name from lat and long over Google Geocoder API
 * @param  String $lat
 * @param  String $long
 * @return [String|false] City name on success, false on fail
 */
function get_location_name_from_lat_long($lat, $long)
{
    $url = sprintf('https://maps.googleapis.com/maps/api/geocode/json?latlng=%s,%s&sensor=true', $lat, $long);
    if (function_exists('curl_init')) {
        $ch = curl_init($url);
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
        $response = curl_exec($ch);
    } else {
        $response = http_get($url);
    }


    if ($response) {
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
