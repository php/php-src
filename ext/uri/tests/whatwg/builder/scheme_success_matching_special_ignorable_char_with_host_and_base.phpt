--TEST--
Test Uri\WhatWg\UrlBuilder::setScheme() - success - matching special scheme containing ignorable characters with host and base URL
--FILE--
<?php

$base = new Uri\WhatWg\Url('https://example.com/base/path?oldQuery#oldFragment');

$errors = [];
$referenceErrors = [];

$reference = new Uri\WhatWg\Url("h\nt\ttps://example.net", $base, $referenceErrors);

$url = new Uri\WhatWg\UrlBuilder()
    ->setScheme("h\nt\ttps")
    ->setHost('example.net')
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
string(20) "https://example.net/"
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
    string(6) "
t	tps"
    ["type"]=>
    enum(Uri\WhatWg\UrlValidationErrorType::InvalidUrlUnit)
    ["failure"]=>
    bool(false)
  }
}
bool(true)
bool(true)
bool(true)
