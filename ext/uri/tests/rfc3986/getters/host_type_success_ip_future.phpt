--TEST--
Test Uri\Rfc3986\Uri component retrieval - host type - IP future
--FILE--
<?php

$uri = Uri\Rfc3986\Uri::parse("https://[vF.addr]");

var_dump($uri->getHostType());

?>
--EXPECT--
enum(Uri\Rfc3986\UriHostType::IPvFuture)
