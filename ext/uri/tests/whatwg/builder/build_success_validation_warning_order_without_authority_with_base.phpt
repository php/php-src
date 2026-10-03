--TEST--
Test Uri\WhatWg\UrlBuilder::build() - success - validation warning order without authority and with base URL
--FILE--
<?php

$base = new Uri\WhatWg\Url('https://example.com/base/path');

$errors = [];
$referenceErrors = [];

$reference = new Uri\WhatWg\Url('/foo\bar#%', $base, $referenceErrors);

$url = new Uri\WhatWg\UrlBuilder()
    ->setPath('/foo\bar')
    ->setFragment('%')
    ->build($base, $errors);

var_dump($url->toAsciiString());
var_dump($url);
var_dump($errors);
var_dump(
    array_map(static fn($error) => $error->type, $errors)
    === array_map(static fn($error) => $error->type, $referenceErrors)
);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));
var_dump($url->equals($reference, Uri\UriComparisonMode::IncludeFragment));

?>
--EXPECTF--
string(29) "https://example.com/foo/bar#%"
object(Uri\WhatWg\Url)#%d (%d) {
  ["scheme"]=>
  string(5) "https"
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(11) "example.com"
  ["port"]=>
  NULL
  ["path"]=>
  string(8) "/foo/bar"
  ["query"]=>
  NULL
  ["fragment"]=>
  string(1) "%"
}
array(2) {
  [0]=>
  object(Uri\WhatWg\UrlValidationError)#%d (%d) {
    ["context"]=>
    string(1) "%"
    ["type"]=>
    enum(Uri\WhatWg\UrlValidationErrorType::InvalidUrlUnit)
    ["failure"]=>
    bool(false)
  }
  [1]=>
  object(Uri\WhatWg\UrlValidationError)#%d (%d) {
    ["context"]=>
    string(4) "\bar"
    ["type"]=>
    enum(Uri\WhatWg\UrlValidationErrorType::InvalidReverseSoldius)
    ["failure"]=>
    bool(false)
  }
}
bool(true)
bool(true)
bool(true)
