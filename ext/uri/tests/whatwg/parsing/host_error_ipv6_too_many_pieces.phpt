--TEST--
Test Uri\WhatWg\Url parsing - host - IPv6 too many pieces
--FILE--
<?php

$errors = [];
$url = Uri\WhatWg\Url::parse('http://[1:2:3:4:5:6:7:8:9]/', errors: $errors);

var_dump($url);
foreach ($errors as $error) {
    echo $error->type->name, ': ', $error->context, ': ';
    var_dump($error->failure);
}

?>
--EXPECT--
NULL
Ipv6TooManyPieces: 9]/: bool(true)
