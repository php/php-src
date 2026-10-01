--TEST--
Test Uri\WhatWg\UrlBuilder::build() - success - keeps a leading double slash in a file path
--FILE--
<?php

$base = new Uri\WhatWg\Url("file:///C:/a/b");
$builder = new Uri\WhatWg\UrlBuilder();
$builder->setPath("//other/x");
$url = $builder->build($base);

var_dump($url->toAsciiString());
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));

?>
--EXPECT--
string(16) "file:////other/x"
bool(true)
