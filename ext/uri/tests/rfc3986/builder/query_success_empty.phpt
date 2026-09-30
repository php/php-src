--TEST--
Test Uri\Rfc3986\UriBuilder::setQuery() - success - empty string
--FILE--
<?php

$uri = new Uri\Rfc3986\UriBuilder()
    ->setQuery('')
    ->build();

var_dump($uri->toRawString());
var_dump($uri);
var_dump($uri->equals(new Uri\Rfc3986\Uri($uri->toRawString())));
var_dump($uri->equals(new Uri\Rfc3986\Uri('?'), Uri\UriComparisonMode::IncludeFragment));

?>
--EXPECTF--
string(1) "?"
object(Uri\Rfc3986\Uri)#%d (%d) {
  ["scheme"]=>
  NULL
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  NULL
  ["port"]=>
  NULL
  ["path"]=>
  string(0) ""
  ["query"]=>
  string(0) ""
  ["fragment"]=>
  NULL
}
bool(true)
bool(true)
