--TEST--
Test Uri\Rfc3986\Uri component modification - port - unsetting existing
--EXTENSIONS--
uri
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse("https://example.com:80");
$uri2 = $uri1->withPort(null);

var_dump($uri1->getPort());
var_dump($uri2->getPort());
var_dump($uri2->toRawString());

?>
--EXPECT--
int(80)
NULL
string(19) "https://example.com"
