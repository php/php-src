--TEST--
Test Uri\Rfc3986\Uri::parse() returns null for a malformed URI
--FILE--
<?php

var_dump(Uri\Rfc3986\Uri::parse("http://RuPaul's Drag Race All Stars 7 Winners Cast on This Season's"));

?>
--EXPECT--
NULL
