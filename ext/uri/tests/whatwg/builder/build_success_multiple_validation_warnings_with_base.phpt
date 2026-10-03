--TEST--
Test Uri\WhatWg\UrlBuilder::build() - success - multiple validation warnings with base URL
--FILE--
<?php

$base = new Uri\WhatWg\Url('https://example.com/base/path');

$errors = [];
$referenceErrors = [];

$reference = new Uri\WhatWg\Url('//127.0.0.1.\newPath', $base, $referenceErrors);

$url = new Uri\WhatWg\UrlBuilder()
    ->setHost('127.0.0.1.')
    ->setPath('\newPath')
    ->build($base, $errors);

var_dump($url->toAsciiString());
var_dump($url);
var_dump($errors);
var_dump($errors == $referenceErrors);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));
var_dump($url->equals($reference, Uri\UriComparisonMode::IncludeFragment));

?>
--EXPECTF--
string(25) "https://127.0.0.1/newPath"
object(Uri\WhatWg\Url)#%d (%d) {
  ["scheme"]=>
  string(5) "https"
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(9) "127.0.0.1"
  ["port"]=>
  NULL
  ["path"]=>
  string(8) "/newPath"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
array(2) {
  [0]=>
  object(Uri\WhatWg\UrlValidationError)#%d (%d) {
    ["context"]=>
    string(8) "\newPath"
    ["type"]=>
    enum(Uri\WhatWg\UrlValidationErrorType::InvalidReverseSoldius)
    ["failure"]=>
    bool(false)
  }
  [1]=>
  object(Uri\WhatWg\UrlValidationError)#%d (%d) {
    ["context"]=>
    string(0) ""
    ["type"]=>
    enum(Uri\WhatWg\UrlValidationErrorType::Ipv4EmptyPart)
    ["failure"]=>
    bool(false)
  }
}
bool(true)
bool(true)
bool(true)
