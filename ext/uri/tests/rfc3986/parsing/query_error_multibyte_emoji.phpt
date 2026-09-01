--TEST--
Test Uri\Rfc3986\Uri parsing - query - emoji code point
--FILE--
<?php

var_dump(Uri\Rfc3986\Uri::parse("https://example.com?🐘=🐘"));

?>
--EXPECT--
NULL
