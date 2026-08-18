--TEST--
Test Uri\WhatWg\Url parsing - host - IPv6 invalid compression
--FILE--
<?php

$errors = [];
$url = Uri\WhatWg\Url::parse('http://[:1]/', errors: $errors);

var_dump($url);
foreach ($errors as $error) {
    echo $error->type->name, ': ', $error->context, ': ';
    var_dump($error->failure);
}

?>
--EXPECT--
NULL
Ipv6InvalidCompression: 1]/: bool(true)
