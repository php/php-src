--TEST--
Test Uri\WhatWg\UrlBuilder::setFragment() - success - empty fragment
--FILE--
<?php

$url = new Uri\WhatWg\UrlBuilder()
    ->setScheme('https')
    ->setHost('example.com')
    ->setFragment('#')
    ->build();

var_dump($url->toAsciiString());
var_dump($url);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));
var_dump($url->equals(new Uri\WhatWg\Url('https://example.com/#'), Uri\UriComparisonMode::IncludeFragment));

?>
--EXPECTF--
string(21) "https://example.com/#"
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
  NULL
  ["fragment"]=>
  string(0) ""
}
bool(true)
bool(true)
