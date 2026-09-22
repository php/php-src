--TEST--
Test Uri\WhatWg\UrlBuilder::build() - success - keeps a leading double slash in the path
--FILE--
<?php

$base = new Uri\WhatWg\Url("https://example.com/a/b");
$builder = new Uri\WhatWg\UrlBuilder();
$builder->setPath("//other/x");
$url = $builder->build($base);

var_dump($url->toAsciiString());
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));

?>
--EXPECT--
string(28) "https://example.com//other/x"
bool(true)
