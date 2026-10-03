--TEST--
Test Uri\WhatWg\UrlBuilder::setScheme() - success - matching special scheme with base URL
--FILE--
<?php

$base = new Uri\WhatWg\Url('https://user:pass@example.com:123/base/path?oldQuery#oldFragment');

$errors = [];
$referenceErrors = [];

$reference = new Uri\WhatWg\Url('https:', $base, $referenceErrors);

$url = new Uri\WhatWg\UrlBuilder()
    ->setScheme('https')
    ->build($base, $errors);

var_dump($url->toAsciiString());
var_dump($url);
var_dump($errors);
var_dump($errors == $referenceErrors);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));
var_dump($url->equals($reference, Uri\UriComparisonMode::IncludeFragment));

?>
--EXPECTF--
string(52) "https://user:pass@example.com:123/base/path?oldQuery"
object(Uri\WhatWg\Url)#%d (%d) {
  ["scheme"]=>
  string(5) "https"
  ["username"]=>
  string(4) "user"
  ["password"]=>
  string(4) "pass"
  ["host"]=>
  string(11) "example.com"
  ["port"]=>
  int(123)
  ["path"]=>
  string(10) "/base/path"
  ["query"]=>
  string(8) "oldQuery"
  ["fragment"]=>
  NULL
}
array(1) {
  [0]=>
  object(Uri\WhatWg\UrlValidationError)#%d (%d) {
    ["context"]=>
    string(0) ""
    ["type"]=>
    enum(Uri\WhatWg\UrlValidationErrorType::SpecialSchemeMissingFollowingSolidus)
    ["failure"]=>
    bool(false)
  }
}
bool(true)
bool(true)
bool(true)
