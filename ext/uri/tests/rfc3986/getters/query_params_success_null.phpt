--TEST--
Test Uri\Rfc3986\Uri::getQueryParams() - success - an absent query returns null
--FILE--
<?php

$uri = new Uri\Rfc3986\Uri("https://example.com");

var_dump($uri->getQueryParams());

?>
--EXPECT--
NULL
