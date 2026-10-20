--TEST--
Test Uri\WhatWg\UrlBuilder::setPassword() - success - empty string with host normalised to empty and file base URL
--FILE--
<?php

$base = new Uri\WhatWg\Url('file:///base/path');

$errors = [];
$referenceErrors = [];

$reference = new Uri\WhatWg\Url("//\t\n", $base, $referenceErrors);

$url = new Uri\WhatWg\UrlBuilder()
    ->setHost("\t\n")
    ->setPassword('')
    ->build($base, $errors);

var_dump($url->toAsciiString());
var_dump($url);
var_dump($errors);
var_dump($errors == $referenceErrors);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));
var_dump($url->equals($reference, Uri\UriComparisonMode::IncludeFragment));

?>
--EXPECTF--
string(8) "file:///"
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
  string(1) "/"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
array(1) {
  [0]=>
  object(Uri\WhatWg\UrlValidationError)#%d (%d) {
    ["context"]=>
    string(2) "%r\x09\x0A%r"
    ["type"]=>
    enum(Uri\WhatWg\UrlValidationErrorType::InvalidUrlUnit)
    ["failure"]=>
    bool(false)
  }
}
bool(true)
bool(true)
bool(true)
