--TEST--
Test Uri\Rfc3986\UriBuilder::setQuery() - success - empty string with base URI
--FILE--
<?php

$base = new Uri\Rfc3986\Uri('https://example.com/base/path?oldQuery#oldFragment');

$uri = new Uri\Rfc3986\UriBuilder()
    ->setQuery('')
    ->build($base);

var_dump($uri->toRawString());
var_dump($uri);
var_dump($uri->equals(new Uri\Rfc3986\Uri($uri->toRawString())));
var_dump($uri->equals(new Uri\Rfc3986\Uri('?', $base), Uri\UriComparisonMode::IncludeFragment));

?>
--EXPECTF--
string(30) "https://example.com/base/path?"
object(Uri\Rfc3986\Uri)#%d (%d) {
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
  string(10) "/base/path"
  ["query"]=>
  string(0) ""
  ["fragment"]=>
  NULL
}
bool(true)
bool(true)
