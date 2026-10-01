--TEST--
Test Uri\WhatWg\UrlBuilder::build() - success - inherits the path and query after removing tabs and newlines
--FILE--
<?php

$base = new Uri\WhatWg\Url("https://example.com/a?old#old");
$builder = new Uri\WhatWg\UrlBuilder();
$builder->setPath("\t\n");
$url = $builder->build($base, $softErrors);

var_dump($url->toAsciiString());
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));
var_dump($softErrors[0]->type);

?>
--EXPECT--
string(25) "https://example.com/a?old"
bool(true)
enum(Uri\WhatWg\UrlValidationErrorType::InvalidUrlUnit)
