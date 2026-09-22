--TEST--
Test Uri\WhatWg\UrlBuilder::build() - success - resolves a path against the base directory and clears its query and fragment
--FILE--
<?php

$base = new Uri\WhatWg\Url("https://user:pass@example.com:81/a/b?old#old");
$builder = new Uri\WhatWg\UrlBuilder();
$builder->setPath("c");
$url = $builder->build($base);

var_dump($url->toAsciiString());
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));

?>
--EXPECT--
string(36) "https://user:pass@example.com:81/a/c"
bool(true)
