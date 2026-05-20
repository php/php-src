--TEST--
Test Uri\Rfc3986\Uri component retrieval - host type - registered name
--FILE--
<?php

$uri = Uri\Rfc3986\Uri::parse("/foo/bar");

var_dump($uri->getHostType());

?>
--EXPECT--
NULL
