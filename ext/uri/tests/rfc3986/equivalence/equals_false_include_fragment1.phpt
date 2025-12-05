--TEST--
Test Uri\Rfc3986\Uri equivalence - returns false - fragment included
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse("https://user:info@example.com:443/foo/bar?abc=123&def=ghi#hashmark");
$uri2 = Uri\Rfc3986\Uri::parse("https://user:info@example.com:443/foo/bar?abc=123&def=ghi#hash");

var_dump($uri1->equals($uri2, Uri\UriComparisonMode::IncludeFragment));
var_dump($uri2->equals($uri1, Uri\UriComparisonMode::IncludeFragment));

?>
--EXPECT--
bool(false)
bool(false)
