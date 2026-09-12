--TEST--
Test Uri\WhatWg\Url parsing - host - IPv4-in-IPv6 too few parts
--FILE--
<?php

$errors = [];
$url = Uri\WhatWg\Url::parse('http://[::ffff:1.2.3]/', errors: $errors);

var_dump($url);
foreach ($errors as $error) {
    echo $error->type->name, ': ', $error->context, ': ';
    var_dump($error->failure);
}

?>
--EXPECT--
NULL
Ipv4InIpv6TooFewParts: ]/: bool(true)
