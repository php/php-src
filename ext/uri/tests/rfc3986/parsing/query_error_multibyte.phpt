--TEST--
Test Uri\Rfc3986\Uri parsing - query - multibyte character
--FILE--
<?php

var_dump(Uri\Rfc3986\Uri::parse("http://example.com?foo=Helló Wörld"));

?>
--EXPECT--
NULL
