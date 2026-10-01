--TEST--
Test Uri\WhatWg\UrlBuilder::build() - success - resolves a parent path segment
--FILE--
<?php

$base = new Uri\WhatWg\Url("https://example.com/a/b");
$builder = new Uri\WhatWg\UrlBuilder();
$builder->setPath("../c");
$url = $builder->build($base);

var_dump($url->toAsciiString());
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));

?>
--EXPECT--
string(21) "https://example.com/c"
bool(true)
