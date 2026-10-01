--TEST--
Test Uri\WhatWg\UrlBuilder::build() - success - keeps a hashmark in the query
--FILE--
<?php

$base = new Uri\WhatWg\Url("https://example.com/a?old#old");
$builder = new Uri\WhatWg\UrlBuilder();
$builder->setQuery("a#b");
$url = $builder->build($base);

var_dump($url->toAsciiString());
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));

?>
--EXPECT--
string(27) "https://example.com/a?a%23b"
bool(true)
