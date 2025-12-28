--TEST--
Test Uri\Rfc3986\Uri component modification - path - using an email format
--EXTENSIONS--
uri
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse("");
$uri2 = $uri1->withPath("john.doe@example.com");

var_dump($uri1->getRawPath());
var_dump($uri2->getRawPath());
var_dump($uri2->toRawString());
var_dump($uri2->getPath());
var_dump($uri2->toString());

?>
--EXPECT--
string(0) ""
string(20) "john.doe@example.com"
string(20) "john.doe@example.com"
string(20) "john.doe@example.com"
string(20) "john.doe@example.com"
