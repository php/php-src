--TEST--
Test Uri\WhatWg\UrlBuilder::setPath() - success - backslash with host and special base URL
--FILE--
<?php

$base = new Uri\WhatWg\Url('https://example.com/base/path');

$errors = [];
$referenceErrors = [];

$reference = new Uri\WhatWg\Url('//example.net\\newPath', $base, $referenceErrors);

$url = new Uri\WhatWg\UrlBuilder()
    ->setHost('example.net')
    ->setPath('\\newPath')
    ->build($base, $errors);

var_dump($url->toAsciiString());
var_dump($url);
var_dump($errors);
var_dump($errors == $referenceErrors);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));
var_dump($url->equals($reference, Uri\UriComparisonMode::IncludeFragment));

?>
--EXPECTF--
string(27) "https://example.net/newPath"
object(Uri\WhatWg\Url)#%d (%d) {
  ["scheme"]=>
  string(5) "https"
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(11) "example.net"
  ["port"]=>
  NULL
  ["path"]=>
  string(8) "/newPath"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
array(1) {
  [0]=>
  object(Uri\WhatWg\UrlValidationError)#%d (%d) {
    ["context"]=>
    string(8) "\newPath"
    ["type"]=>
    enum(Uri\WhatWg\UrlValidationErrorType::InvalidReverseSoldius)
    ["failure"]=>
    bool(false)
  }
}
bool(true)
bool(true)
bool(true)
