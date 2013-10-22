<?php

$redis_connection_settings = array(

);

require 'Predis/Autoloader.php';

Predis\Autoloader::register();

return new Predis\Client($redis_connection_settings);
