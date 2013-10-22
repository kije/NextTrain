<?php

// replace this with your path to Predis
require '/home/kije/pear/php/Predis/Autoloader.php';

Predis\Autoloader::register();

class rDB {


    static function store($key, $value, $ttl) {
        $client = include 'db_connect.php';
        $client->set($key, $value);
        $client->expire($key, $ttl);
    }

    static function exists($key) {
        $client = include 'db_connect.php';
        return !!$client->exists($key);
    }

    static function retrive($key) {
        $client = include 'db_connect.php';
        return $client->get($key);
    }
}