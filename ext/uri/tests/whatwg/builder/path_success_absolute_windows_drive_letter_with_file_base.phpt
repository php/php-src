--TEST--
Test Uri\WhatWg\UrlBuilder::setPath() - success - absolute Windows drive letter with file base URL
--FILE--
<?php

$base = new Uri\WhatWg\Url('file:///C:/base/path?oldQuery#oldFragment');

$errors = [];
$referenceErrors = [];

$reference = new Uri\WhatWg\Url('/D:/newPath', $base, $referenceErrors);

$url = new Uri\WhatWg\UrlBuilder()
    ->setPath('/D:/newPath')
    ->build($base, $errors);

var_dump($url->toAsciiString());
var_dump($url);
var_dump($errors);
var_dump($errors == $referenceErrors);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));
var_dump($url->equals($reference, Uri\UriComparisonMode::IncludeFragment));

?>
--EXPECTF--
string(18) "file:///D:/newPath"
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
  string(11) "/D:/newPath"
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
