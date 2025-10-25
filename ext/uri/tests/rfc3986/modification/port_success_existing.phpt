--TEST--
Test Uri\Rfc3986\Uri component modification - port - changing an existing one
--EXTENSIONS--
uri
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse("https://example.com:80");
$uri2 = $uri1->withPort(443);

var_dump($uri1->getPort());
var_dump($uri2->getPort());
var_dump($uri2->toRawString());

?>
--EXPECT--
int(80)
int(443)
string(23) "https://example.com:443"
