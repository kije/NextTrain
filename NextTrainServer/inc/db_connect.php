<?php

// Replace this with your Settings
$redis_connection_settings = array(
    'scheme' => 'unix',
    'path' => '/home/kije/.redis/sock'
);


return new Predis\Client($redis_connection_settings);
