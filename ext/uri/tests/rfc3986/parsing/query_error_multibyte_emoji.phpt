--TEST--
Test Uri\Rfc3986\Uri parsing - query - emoji character
--FILE--
<?php

var_dump(Uri\Rfc3986\Uri::parse("https://example.com?🐘=🐘"));

?>
--EXPECT--
NULL
