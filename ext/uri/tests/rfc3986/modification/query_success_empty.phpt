--TEST--
Test Uri\Rfc3986\Uri component modification - query - empty string
--EXTENSIONS--
uri
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse("https://example.com");
$uri2 = $uri1->withQuery("");

var_dump($uri1->getRawQuery());
var_dump($uri2->getRawQuery());
var_dump($uri2->getQuery());

?>
--EXPECT--
NULL
string(0) ""
string(0) ""
