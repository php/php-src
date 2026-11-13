--TEST--
Test Uri\WhatWg\Url parsing - host - IPv4 non-numeric part
--FILE--
<?php

$errors = [];
$url = Uri\WhatWg\Url::parse('http://1.a.2.3/', errors: $errors);

var_dump($url);
foreach ($errors as $error) {
    echo $error->type->name, ': ', $error->context, ': ';
    var_dump($error->failure);
}

?>
--EXPECT--
NULL
Ipv4NonNumericPart: a.2.3: bool(true)
