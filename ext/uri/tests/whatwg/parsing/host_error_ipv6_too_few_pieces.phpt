--TEST--
Test Uri\WhatWg\Url parsing - host - IPv6 too few pieces
--FILE--
<?php

$errors = [];
$url = Uri\WhatWg\Url::parse('http://[1:2:3]/', errors: $errors);

var_dump($url);
foreach ($errors as $error) {
    echo $error->type->name, ': ', $error->context, ': ';
    var_dump($error->failure);
}

?>
--EXPECT--
NULL
Ipv6TooFewPieces: ]/: bool(true)
