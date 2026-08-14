--TEST--
Test Uri\Rfc3986\Uri component modification - port - unsetting without a host
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse("/foo");
$uri2 = $uri1->withPort(null);

var_dump($uri2->getPort());

?>
--EXPECT--
NULL
