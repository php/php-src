--TEST--
Test Uri\WhatWg\UrlBuilder::build() - success - replaces the base query with an empty query
--FILE--
<?php

$base = new Uri\WhatWg\Url("https://example.com/a?old#old");
$builder = new Uri\WhatWg\UrlBuilder();
$builder->setQuery("");
$url = $builder->build($base);

var_dump($url->toAsciiString());
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));

?>
--EXPECT--
string(22) "https://example.com/a?"
bool(true)
