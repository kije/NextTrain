<?php
require_once 'globals.php';


function test_get_location_name_from_lat_long()
{
    $res = get_location_name_from_lat_long('47.484456', '7.731361');
    return !empty($res) && $res === 'Liestal';
}

echo 'Test get_location_name_from_lat_long()'.PHP_EOL;
echo (test_get_location_name_from_lat_long() ? 'passed' : 'not_passed').PHP_EOL.PHP_EOL;
