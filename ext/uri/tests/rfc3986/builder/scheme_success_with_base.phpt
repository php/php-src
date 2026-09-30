--TEST--
Test Uri\Rfc3986\UriBuilder::setScheme() - success - with base URI
--FILE--
<?php

$base = new Uri\Rfc3986\Uri('https://example.com/base/path');

$uri = new Uri\Rfc3986\UriBuilder()
    ->setScheme('http')
    ->build($base);

var_dump($uri->toRawString());
var_dump($uri);
var_dump($uri->equals(new Uri\Rfc3986\Uri($uri->toRawString())));
var_dump($uri->equals(new Uri\Rfc3986\Uri('http:', $base), Uri\UriComparisonMode::IncludeFragment));

?>
--EXPECTF--
string(5) "http:"
object(Uri\Rfc3986\Uri)#%d (%d) {
  ["scheme"]=>
  string(4) "http"
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
  NULL
  ["fragment"]=>
  NULL
}
bool(true)
bool(true)
