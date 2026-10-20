--TEST--
Test Uri\WhatWg\UrlBuilder::build() - success - keeps question mark and hashmark in the path
--FILE--
<?php

$base = new Uri\WhatWg\Url("https://example.com/a/b");
$builder = new Uri\WhatWg\UrlBuilder();
$builder->setPath("a?b#c");
$url = $builder->build($base);

var_dump($url->toAsciiString());
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));

?>
--EXPECT--
string(31) "https://example.com/a/a%3Fb%23c"
bool(true)
