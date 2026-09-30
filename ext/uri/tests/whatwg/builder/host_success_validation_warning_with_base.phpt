--TEST--
Test Uri\WhatWg\UrlBuilder::setHost() - success - validation warning with base URL
--FILE--
<?php

$base = new Uri\WhatWg\Url('https://example.com/base/path');

$errors = [];
$referenceErrors = [];

$reference = new Uri\WhatWg\Url('//127.0.0.1.', $base, $referenceErrors);

$url = new Uri\WhatWg\UrlBuilder()
    ->setHost('127.0.0.1.')
    ->build($base, $errors);

var_dump($url->toAsciiString());
var_dump($url);
var_dump($errors);
var_dump($errors == $referenceErrors);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));
var_dump($url->equals($reference, Uri\UriComparisonMode::IncludeFragment));

?>
--EXPECTF--
string(18) "https://127.0.0.1/"
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
