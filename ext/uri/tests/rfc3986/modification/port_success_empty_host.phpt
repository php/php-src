--TEST--
Test Uri\Rfc3986\Uri component modification - port - adding with an empty host
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse("file:///foo/bar");
$uri2 = $uri1->withPort(80);

var_dump($uri1->getPort());
var_dump($uri2->getPort());

?>
--EXPECT--
NULL
int(80)
