<?php
/**
 * Outputs a JSON object with
 *
 *  @arg lat    the Latitude
 *  @arg long   the Longnitude
 *  @arg limit  limit
 */
require_once 'inc/globals.php';

header('Content-Type: application/json');

$output = array(
    PEBBLE_STATUS_KEY => http_response_code(),
);

$allowed_params = array(
    'lat' => '',
    'long' => '',
    'limit' => '',
);


$params = array_intersect_key($_POST, $allowed_params);


if (array_key_exists('lat', $params) && array_key_exists('long', $params)) {
    $locality = get_location_name_from_lat_long($params['lat'], $params['long']);
    $stationboard = get_stationboard(
        $locality,
        (array_key_exists('limit', $params) ? min($params['limit'], (PEBBLE_RESULT_END_KEY - PEBBLE_RESULT_START_KEY)/PEBBLE_RESERVED_FIELDS_PER_RESULT) : 5)
    );

    $i = PEBBLE_RESULT_START_KEY;
    foreach ($stationboard as $index => $data) {
        if ($i > PEBBLE_RESULT_END_KEY) {
            break;
        }

        $output[$i+PEBBLE_RESULT_NTH_SATION_NAME]   = $data['stop']['station']['name'];
        $output[$i+PEBBLE_RESULT_NTH_DELAY]         = (int)$data['stop']['delay'];
        $output[$i+PEBBLE_RESULT_NTH_PLATFORM]      = $data['stop']['platform'];
        $output[$i+PEBBLE_RESULT_NTH_TO]            = $data['to'];
        $output[$i+PEBBLE_RESULT_NTH_CATEGORY]      = $data['category'];

        // Time
        $time = strtotime($data['stop']['departure']);
        $output[$i+PEBBLE_RESULT_NTH_DEPARTURE_HOUR]    = (int)date('G', $time); // 24h hour
        $output[$i+PEBBLE_RESULT_NTH_DEPARTURE_MIN]     = (int)date('i', $time); // minute
        $output[$i+PEBBLE_RESULT_NTH_DEPARTURE_DST]     = (int)date('I', $time); // DST
        $output[$i+PEBBLE_RESULT_NTH_DEPARTURE_DAY]     = (int)date('j', $time); // Day
        $output[$i+PEBBLE_RESULT_NTH_DEPARTURE_MONTH]   = (int)date('n', $time); // Month

        $i += PEBBLE_RESERVED_FIELDS_PER_RESULT;
    }
} else {
    $output[PEBBLE_ERROR_KEY] = 'No coordinates specified!';
}

echo json_encode($output);