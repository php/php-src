--TEST--
Test Uri\Rfc3986\Uri equivalence - returns true - after multiple normalization steps
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse("https://user:info@example%2ecom:443/foo/bar?abc=123&def=ghi#hashmark");
$uri2 = Uri\Rfc3986\Uri::parse("HTTPS://user:info@EXAMPLE.COM:0443/../foo/b%61r?abc=123&def=ghi#hashmark");

var_dump($uri1->equals($uri2));
var_dump($uri2->equals($uri1));

?>
--EXPECT--
bool(true)
bool(true)
