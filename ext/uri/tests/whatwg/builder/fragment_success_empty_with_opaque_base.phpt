--TEST--
Test Uri\WhatWg\UrlBuilder::build() - success - clears the fragment of an opaque base with an empty string
--FILE--
<?php

$base = new Uri\WhatWg\Url("foo:opaque?old#old");
$builder = new Uri\WhatWg\UrlBuilder();
$builder->setPath("\t\n");
$builder->setFragment("");
$url = $builder->build($base, $softErrors);
var_dump($url->toAsciiString());
var_dump($softErrors[0]->type);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));
var_dump($base->toAsciiString());

?>
--EXPECT--
string(14) "foo:opaque?old"
enum(Uri\WhatWg\UrlValidationErrorType::InvalidUrlUnit)
bool(true)
string(18) "foo:opaque?old#old"
