--TEST--
Test Uri\Rfc3986\Uri component modification - port - unsetting non-existent
--EXTENSIONS--
uri
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse("ftp://example.com");
$uri2 = $uri1->withPort(null);

var_dump($uri2->getPort());
var_dump($uri2->getPort());
var_dump($uri2->toRawString());

?>
--EXPECT--
NULL
NULL
string(17) "ftp://example.com"
