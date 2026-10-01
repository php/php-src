--TEST--
Test Uri\WhatWg\UrlBuilder::build() - success - inherits the path and query but not the fragment
--FILE--
<?php

$base = new Uri\WhatWg\Url("https://example.com/a?old#old");
$builder = new Uri\WhatWg\UrlBuilder();

$url = $builder->build($base);

var_dump($url->toAsciiString());
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));

?>
--EXPECT--
string(25) "https://example.com/a?old"
bool(true)
