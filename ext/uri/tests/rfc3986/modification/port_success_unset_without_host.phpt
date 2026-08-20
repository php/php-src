--TEST--
Test Uri\Rfc3986\Uri component modification - port - unsetting without a host
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse('https://:80/foo/bar');
$uri2 = $uri1->withPort(null);

var_dump($uri1->getPort());
var_dump($uri1->toString());

var_dump($uri2->getPort());
var_dump($uri2->toString());

?>
--EXPECT--
int(80)
string(19) "https://:80/foo/bar"
NULL
string(16) "https:///foo/bar"
