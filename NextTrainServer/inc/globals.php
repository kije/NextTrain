<?php

define('SERVROOT', realpath(dirname(__DIR__).'/..'));
define('INCROOT', __DIR__);

define('DEBUG', false);

ini_set('display_errors', DEBUG);
error_reporting((DEBUG ? E_ALL : E_ERROR));

@session_start();

define('ENCODING', 'UTF-8');
mb_internal_encoding(ENCODING);

require_once INCROOT.'/db.php';
require_once INCROOT.'/functions.php';



define('PEBBLE_STATUS_KEY', 1);
define('PEBBLE_ERROR_KEY', 2);
define('PEBBLE_RESULT_START_KEY', 10);
define('PEBBLE_RESULT_END_KEY', 200);
define('PEBBLE_RESERVED_FIELDS_PER_RESULT', 19);
define('PEBBLE_RESULT_NTH_SATION_NAME', 0); // which field in the PEBBLE_RESERVED_FIELDS_PER_RESULT reserved fields is station name -> no nesting supported by httpebble
define('PEBBLE_RESULT_NTH_DELAY', 1);
define('PEBBLE_RESULT_NTH_PLATFORM', 2);
define('PEBBLE_RESULT_NTH_TO', 3);
define('PEBBLE_RESULT_NTH_CATEGORY', 4);

define('PEBBLE_RESULT_NTH_DEPARTURE_HOUR', 5);
define('PEBBLE_RESULT_NTH_DEPARTURE_MIN', 6);
define('PEBBLE_RESULT_NTH_DEPARTURE_DST', 7);
define('PEBBLE_RESULT_NTH_DEPARTURE_DAY', 8);
define('PEBBLE_RESULT_NTH_DEPARTURE_MONTH', 9);

