--TEST--
Test Uri\WhatWg\Url parsing - host - IPv4 out-of-range part
--FILE--
<?php

$errors = [];
$url = Uri\WhatWg\Url::parse('http://256.0.0.1/', errors: $errors);

var_dump($url);
foreach ($errors as $error) {
    echo $error->type->name, ': ', $error->context, ': ';
    var_dump($error->failure);
}

?>
--EXPECT--
NULL
Ipv4OutOfRangePart: 256.0.0.1: bool(true)
