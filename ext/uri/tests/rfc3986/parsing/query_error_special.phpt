--TEST--
Test Uri\Rfc3986\Uri parsing - query - special characters
--FILE--
<?php

var_dump(Uri\Rfc3986\Uri::parse("http://example.com?foobar='<script> + @"));

?>
--EXPECT--
NULL
