<?php
require_once 'inc/globals.php';

header('Content-Type: application/json');

echo json_encode(array(
    1 => http_response_code(),
));
