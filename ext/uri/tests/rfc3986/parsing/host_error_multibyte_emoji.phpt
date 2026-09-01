--TEST--
Test Uri\Rfc3986\Uri parsing - host - emoji codepoint
--FILE--
<?php

var_dump(Uri\Rfc3986\Uri::parse("https://🐘.com"));

?>
--EXPECT--
NULL
