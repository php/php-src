--TEST--
Test Uri\WhatWg\Url parsing - host - IPv4-in-IPv6 invalid code point
--FILE--
<?php

$errors = [];
$url = Uri\WhatWg\Url::parse('http://[::ffff:1.2.3.a]/', errors: $errors);

var_dump($url);
foreach ($errors as $error) {
    echo $error->type->name, ': ', $error->context, ': ';
    var_dump($error->failure);
}

?>
--EXPECT--
NULL
Ipv4InIpv6InvalidCodePoint: a]/: bool(true)
