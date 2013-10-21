<?php
require_once 'inc/globals.php';

header('Content-Type: application/json');

$output = array(
    1 => http_response_code(),
);

$allowed_params = array(
    'lat' => '',
    'long' => '',
    'limit' = '',
);

$params = array_intersect_key($_POST, $allowed_params);

if (array_key_exists('lat', $params) && array_key_exists('long', $params)) {
    $locality = get_location_name_from_lat_long();
} else {
    $output[2] =
}