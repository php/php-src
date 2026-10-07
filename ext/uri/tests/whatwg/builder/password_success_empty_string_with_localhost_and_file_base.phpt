--TEST--
Test Uri\WhatWg\UrlBuilder::setPassword() - success - empty string with localhost and file base URL
--FILE--
<?php

$base = new Uri\WhatWg\Url('file:///base/path');

$errors = [];
$referenceErrors = [];

$reference = new Uri\WhatWg\Url('//localhost', $base, $referenceErrors);

$url = new Uri\WhatWg\UrlBuilder()
    ->setHost('localhost')
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
array(0) {
}
bool(true)
bool(true)
bool(true)
