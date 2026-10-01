--TEST--
Test Uri\WhatWg\UrlBuilder::build() - success - keeps a colon in the first path segment
--FILE--
<?php

$base = new Uri\WhatWg\Url("https://example.com/a/b");
$builder = new Uri\WhatWg\UrlBuilder();
$builder->setPath("c:d");
$url = $builder->build($base);

var_dump($url->toAsciiString());
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));

?>
--EXPECT--
string(25) "https://example.com/a/c:d"
bool(true)
