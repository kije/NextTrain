<?php
require_once 'inc/globals.php';

header('Content-Type: application/json');

echo json_encode(array(
    PEBBLE_STATUS_KEY => http_response_code(),
));
