--TEST--
Test Uri\Rfc3986\Uri component modification - host - URL encoded reserved characters
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse("https://example.com");
$uri1->withHost("t%3As%2Ft.com");

?>
--EXPECT--
