--TEST--
Test Uri\WhatWg\UrlBuilder::setQuery() - success - strips a single leading question mark
--FILE--
<?php

$errors = [];

$url = new Uri\WhatWg\UrlBuilder()
    ->setScheme('https')
    ->setHost('example.com')
    ->setQuery('??a')
    ->build(softErrors: $errors);

var_dump($url->toAsciiString());
var_dump($url);
var_dump($errors);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString()), Uri\UriComparisonMode::IncludeFragment));

?>
--EXPECTF--
string(23) "https://example.com/??a"
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
  string(1) "/"
  ["query"]=>
  string(2) "?a"
  ["fragment"]=>
  NULL
}
array(0) {
}
bool(true)
