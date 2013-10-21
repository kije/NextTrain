<?php

define('SERVROOT', realpath(dirname(__DIR__).'/..'));
define('INCROOT', __DIR__);

define('DEBUG', true);

ini_set('display_errors', DEBUG);
error_reporting(E_ALL);

@session_start();

define('ENCODING', 'UTF-8');
mb_internal_encoding(ENCODING);

require_once INCROOT.'/functions.php';


define('PEBBLE_STATUS_KEY', 1);
define('PEBBLE_ERROR_KEY', 2);
define('PEBBLE_RESULT_START_KEY', 10);
define('PEBBLE_RESULT_END_KEY', 40);

