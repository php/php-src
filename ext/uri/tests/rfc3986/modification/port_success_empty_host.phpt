--TEST--
Test Uri\Rfc3986\Uri component modification - port - adding with an empty host
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse("file:///foo/bar");
$uri2 = $uri1->withPort(80);

var_dump($uri1->getPort());
var_dump($uri1->toRawString());

var_dump($uri2->getPort());
var_dump($uri2->toRawString());

?>
--EXPECT--
NULL
string(15) "file:///foo/bar"
int(80)
string(18) "file://:80/foo/bar"
