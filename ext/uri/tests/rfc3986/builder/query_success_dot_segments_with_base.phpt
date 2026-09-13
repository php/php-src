--TEST--
Test Uri\Rfc3986\UriBuilder::setQuery() - success - dot segments are data with base URI
--FILE--
<?php

$base = new Uri\Rfc3986\Uri('http://a/b/c/d;p?q');

$uri = new Uri\Rfc3986\UriBuilder()
    ->setPath('g')
    ->setQuery('y/../x')
    ->build($base);

var_dump($uri->toRawString());
var_dump($uri);
var_dump($uri->equals(new Uri\Rfc3986\Uri($uri->toRawString())));
var_dump($uri->equals(new Uri\Rfc3986\Uri('g?y/../x', $base), Uri\UriComparisonMode::IncludeFragment));

?>
--EXPECTF--
string(21) "http://a/b/c/g?y/../x"
object(Uri\Rfc3986\Uri)#%d (%d) {
  ["scheme"]=>
  string(4) "http"
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(1) "a"
  ["port"]=>
  NULL
  ["path"]=>
  string(6) "/b/c/g"
  ["query"]=>
  string(6) "y/../x"
  ["fragment"]=>
  NULL
}
bool(true)
bool(true)
