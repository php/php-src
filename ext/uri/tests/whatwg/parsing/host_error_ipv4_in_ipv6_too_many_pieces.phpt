--TEST--
Test Uri\WhatWg\Url parsing - host - IPv4-in-IPv6 too many pieces
--FILE--
<?php

$errors = [];
$url = Uri\WhatWg\Url::parse('http://[1:2:3:4:5:6:7:1.2.3.4]/', errors: $errors);

var_dump($url);
foreach ($errors as $error) {
    echo $error->type->name, ': ', $error->context, ': ';
    var_dump($error->failure);
}

?>
--EXPECT--
NULL
Ipv4InIpv6TooManyPieces: 1.2.3.4]/: bool(true)
