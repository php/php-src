--TEST--
Test Uri\Rfc3986\UriBuilder::setHost() - success - empty host creates empty authority
--FILE--
<?php

$uri = new Uri\Rfc3986\UriBuilder()->setHost('')->build();

var_dump($uri->getHost());
var_dump($uri->toRawString());

?>
--EXPECT--
string(0) ""
string(2) "//"
