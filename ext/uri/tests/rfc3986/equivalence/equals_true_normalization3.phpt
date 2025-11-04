--TEST--
Test Uri\Rfc3986\Uri equivalence - returns true - after port normalization
--FILE--
<?php

$uri1 = new Uri\Rfc3986\Uri("https://example.com:443");
$uri2 = new Uri\Rfc3986\Uri("https://example.com:0443");

var_dump($uri1->equals($uri2));
var_dump($uri2->equals($uri1));

?>
--EXPECT--
bool(true)
bool(true)
