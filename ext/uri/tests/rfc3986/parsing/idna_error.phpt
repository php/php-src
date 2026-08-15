--TEST--
Test Uri\Rfc3986\Uri parsing - IDNA characters
--FILE--
<?php

var_dump(Uri\Rfc3986\Uri::parse("🐘"));
var_dump(Uri\Rfc3986\Uri::parse("https://🐘.com/🐘?🐘=🐘"));

?>
--EXPECT--
NULL
NULL
