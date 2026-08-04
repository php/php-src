--TEST--
Test Uri\Rfc3986\Uri getter - uri type - URI without authority
--FILE--
<?php

$uri = Uri\Rfc3986\Uri::parse("https:");

var_dump($uri->getUriType());

?>
--EXPECT--
enum(Uri\Rfc3986\UriType::Uri)
