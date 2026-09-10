--TEST--
Test Uri\WhatWg\UrlBuilder::setScheme() - success - matching file scheme with base URL
--FILE--
<?php

$base = new Uri\WhatWg\Url('file:///base/path?oldQuery#oldFragment');

$errors = [];
$referenceErrors = [];

$reference = new Uri\WhatWg\Url('file:', $base, $referenceErrors);

$url = new Uri\WhatWg\UrlBuilder()
    ->setScheme('file')
    ->build($base, $errors);

var_dump($url->toAsciiString());
var_dump($url);
var_dump($errors);
var_dump($errors == $referenceErrors);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));
var_dump($url->equals($reference, Uri\UriComparisonMode::IncludeFragment));

?>
--EXPECTF--
string(26) "file:///base/path?oldQuery"
object(Uri\WhatWg\Url)#%d (%d) {
  ["scheme"]=>
  string(4) "file"
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(0) ""
  ["port"]=>
  NULL
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
