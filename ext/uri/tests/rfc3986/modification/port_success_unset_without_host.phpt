--TEST--
Test Uri\Rfc3986\Uri component modification - port - success - unsetting with an empty host
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse('https://:80/foo/bar');
$uri2 = $uri1->withPort(null);

var_dump($uri1->getPort());

var_dump($uri2->getPort());
var_dump($uri2->toRawString());

?>
--EXPECT--
int(80)
NULL
string(16) "https:///foo/bar"
