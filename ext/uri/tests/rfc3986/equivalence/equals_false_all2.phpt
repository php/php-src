--TEST--
Test Uri\Rfc3986\Uri equivalence - returns false - all components
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse("https://user:info@example.com:443/foo/bar?abc=123&def=ghi#hashmark");
$uri2 = Uri\Rfc3986\Uri::parse("https://user@example.com:443/foo/bar?abc=123&def=ghi#hashmark");

var_dump($uri1->equals($uri2));
var_dump($uri2->equals($uri1));

?>
--EXPECT--
bool(false)
bool(false)
