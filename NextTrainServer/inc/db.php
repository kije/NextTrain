<?php

// prepend a base path if Predis is not present in your "include_path".

require 'Predis/Autoloader.php';

Predis\Autoloader::register();