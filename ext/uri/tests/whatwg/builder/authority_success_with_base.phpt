--TEST--
Test Uri\WhatWg\UrlBuilder::build() - success - replaces the base authority with encoded credentials
--FILE--
<?php

$base = new Uri\WhatWg\Url("https://old:secret@example.com:81/a?old#old");
$builder = new Uri\WhatWg\UrlBuilder();
$builder->setHost("example.net");
$builder->setUsername("a@b");
$builder->setPassword("c:d");
$url = $builder->build($base);

var_dump($url->toAsciiString());
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));
var_dump($base->toAsciiString());

?>
--EXPECT--
string(32) "https://a%40b:c%3Ad@example.net/"
bool(true)
string(43) "https://old:secret@example.com:81/a?old#old"
