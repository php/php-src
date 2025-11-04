--TEST--
Test Uri\Rfc3986\Uri equivalence - returns true - host is percent-decoded during normalization
--FILE--
<?php

$uri1 = new Uri\Rfc3986\Uri("https://example%2ecom/foo/bar");
$uri2 = new Uri\Rfc3986\Uri("https://example.com/foo/bar");

var_dump($uri1->equals($uri2));
var_dump($uri2->equals($uri1));

?>
--EXPECT--
bool(true)
bool(true)
