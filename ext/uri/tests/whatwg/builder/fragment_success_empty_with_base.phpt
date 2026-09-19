--TEST--
Test Uri\WhatWg\UrlBuilder::build() - success - replaces the base fragment with an empty fragment
--FILE--
<?php

$base = new Uri\WhatWg\Url("https://example.com/a?old#old");
$builder = new Uri\WhatWg\UrlBuilder();
$builder->setFragment("");
$url = $builder->build($base);

var_dump($url->toAsciiString());
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));

?>
--EXPECT--
string(26) "https://example.com/a?old#"
bool(true)
