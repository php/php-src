--TEST--
Test Uri\WhatWg\UrlBuilder::build() - success - replaces the file drive
--FILE--
<?php

$base = new Uri\WhatWg\Url("file:///C:/a/b");
$builder = new Uri\WhatWg\UrlBuilder();
$builder->setPath("D:/c");
$url = $builder->build($base);

var_dump($url->toAsciiString());
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));

?>
--EXPECT--
string(12) "file:///D:/c"
bool(true)
