<?php

// prepend a base path if Predis is not present in your "include_path".

$client = require_once 'db_connect.php';


function store($key, $value, $ttl) {
    $client->set($key, $value);
    $client->expire($key, $ttl);
}

function exists($key) {
    return !!$client->exits($key);
}

function retrive($key) {
    return $client->get($key);
}