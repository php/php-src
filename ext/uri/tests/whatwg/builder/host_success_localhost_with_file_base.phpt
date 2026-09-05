--TEST--
Test Uri\WhatWg\UrlBuilder::setHost() - success - localhost with file base URL
--FILE--
<?php

$base = new Uri\WhatWg\Url('file:///C:/base/path?oldQuery#oldFragment');

$errors = [];
$referenceErrors = [];

$reference = new Uri\WhatWg\Url('//localhost/newPath', $base, $referenceErrors);

$url = new Uri\WhatWg\UrlBuilder()
    ->setHost('localhost')
    ->setPath('/newPath')
    ->build($base, $errors);

var_dump($url->toAsciiString());
var_dump($url);
var_dump($errors);
var_dump($errors == $referenceErrors);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));
var_dump($url->equals($reference, Uri\UriComparisonMode::IncludeFragment));

?>
--EXPECTF--
string(15) "file:///newPath"
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
  string(8) "/newPath"
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
