--TEST--
Test Uri\WhatWg\Url parsing - host - IPv4-in-IPv6 out-of-range part
--FILE--
<?php

$errors = [];
$url = Uri\WhatWg\Url::parse('http://[::ffff:256.2.3.4]/', errors: $errors);

var_dump($url);
foreach ($errors as $error) {
    echo $error->type->name, ': ', $error->context, ': ';
    var_dump($error->failure);
}

?>
--EXPECT--
NULL
Ipv4InIpv6OutOfRangePart: 6.2.3.4]/: bool(true)
