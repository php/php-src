--TEST--
Test Uri\Rfc3986\Uri equivalence - returns true - after IPv6 normalization
--FILE--
<?php

$uri1 = new Uri\Rfc3986\Uri("https://[2001:0db8:0001:0000:0000:0ab9:C0A8:0102]");
$uri2 = new Uri\Rfc3986\Uri("https://[2001:DB8:1::AB9:C0A8:102]");

var_dump($uri1->equals($uri2));
var_dump($uri2->equals($uri1));

?>
--EXPECT--
bool(true)
bool(true)
