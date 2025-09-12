--TEST--
Test Uri\Rfc3986\Uri component modification - port - changing an existing one
--EXTENSIONS--
uri
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse("https://example.com:80");
$uri2 = $uri1->withPort(433);

var_dump($uri1->getPort());
var_dump($uri2->getPort());

?>
--EXPECT--
int(80)
int(433)
