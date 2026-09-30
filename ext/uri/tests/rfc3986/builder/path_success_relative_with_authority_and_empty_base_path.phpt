--TEST--
Test Uri\Rfc3986\UriBuilder::setPath() - success - relative path with authority and empty base path
--FILE--
<?php

$base = new Uri\Rfc3986\Uri('https://example.com');

$uri = new Uri\Rfc3986\UriBuilder()
    ->setPath('child')
    ->build($base);

var_dump($uri->toRawString());
var_dump($uri);
var_dump($uri->equals(new Uri\Rfc3986\Uri($uri->toRawString())));
var_dump($uri->equals(new Uri\Rfc3986\Uri('child', $base), Uri\UriComparisonMode::IncludeFragment));

?>
--EXPECTF--
string(25) "https://example.com/child"
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
  string(6) "/child"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
bool(true)
bool(true)
