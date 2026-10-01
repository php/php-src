--TEST--
Test Uri\Rfc3986\Uri parsing - basic - parse() returns null for invalid URIs
--FILE--
<?php

var_dump(Uri\Rfc3986\Uri::parse(':'));
var_dump(Uri\Rfc3986\Uri::parse("https://你好你好"));
var_dump(Uri\Rfc3986\Uri::parse("https://０Ｘｃ０．０２５０．０１"));

?>
--EXPECT--
NULL
NULL
NULL
