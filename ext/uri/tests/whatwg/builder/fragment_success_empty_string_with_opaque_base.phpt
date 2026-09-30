--TEST--
Test Uri\WhatWg\UrlBuilder::setFragment() - success - empty string with base URL containing opaque path
--FILE--
<?php

$base = new Uri\WhatWg\Url('scheme:opaquePath?oldQuery#oldFragment');

$errors = [];
$referenceErrors = [];

$reference = new Uri\WhatWg\Url('#', $base, $referenceErrors);

$url = new Uri\WhatWg\UrlBuilder()
    ->setFragment('')
    ->build($base, $errors);

var_dump($url->toAsciiString());
var_dump($url);
var_dump($errors);
var_dump($errors == $referenceErrors);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString()), Uri\UriComparisonMode::IncludeFragment));
var_dump($url->equals($reference, Uri\UriComparisonMode::IncludeFragment));

?>
--EXPECTF--
string(27) "scheme:opaquePath?oldQuery#"
object(Uri\WhatWg\Url)#%d (%d) {
  ["scheme"]=>
  string(6) "scheme"
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  NULL
  ["port"]=>
  NULL
  ["path"]=>
  string(10) "opaquePath"
  ["query"]=>
  string(8) "oldQuery"
  ["fragment"]=>
  string(0) ""
}
array(0) {
}
bool(true)
bool(true)
bool(true)
