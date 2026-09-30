--TEST--
Test Uri\WhatWg\UrlBuilder::setPath() - success - relative path with hostless non-special base URL
--FILE--
<?php

$base = new Uri\WhatWg\Url('scheme:/base/path?oldQuery#oldFragment');

$errors = [];
$referenceErrors = [];

$reference = new Uri\WhatWg\Url('next', $base, $referenceErrors);

$url = new Uri\WhatWg\UrlBuilder()
    ->setPath('next')
    ->build($base, $errors);

var_dump($url->toAsciiString());
var_dump($url);
var_dump($errors);
var_dump($errors == $referenceErrors);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));
var_dump($url->equals($reference, Uri\UriComparisonMode::IncludeFragment));

?>
--EXPECTF--
string(17) "scheme:/base/next"
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
  string(10) "/base/next"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
array(0) {
}
bool(true)
bool(true)
bool(true)
