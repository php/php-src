--TEST--
Test Uri\WhatWg\Url equivalence - different fragment - include fragment variation
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("https://user:info@example.com:443/foo/bar?abc=123&def=ghi#hashmark");
$url2 = Uri\WhatWg\Url::parse("https://user:info@example.com:443/foo/bar?abc=123&def=ghi#hash");

var_dump($url1->equals($url2, Uri\UriComparisonMode::IncludeFragment));
var_dump($url2->equals($url1, Uri\UriComparisonMode::IncludeFragment));

var_dump($url1->equals($url2, Uri\UriComparisonMode::ExcludeFragment));
var_dump($url2->equals($url1, Uri\UriComparisonMode::ExcludeFragment));

?>
--EXPECT--
bool(false)
bool(false)
bool(true)
bool(true)
