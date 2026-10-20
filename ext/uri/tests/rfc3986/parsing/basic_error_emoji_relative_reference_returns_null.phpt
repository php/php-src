--TEST--
Test Uri\Rfc3986\Uri parsing - basic - parse() returns null for an emoji relative reference
--FILE--
<?php

var_dump(Uri\Rfc3986\Uri::parse("🐘"));

?>
--EXPECT--
NULL
