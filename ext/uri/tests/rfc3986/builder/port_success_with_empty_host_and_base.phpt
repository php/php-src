--TEST--
Test Uri\Rfc3986\UriBuilder::setPort() - success - with empty host and base URI
--FILE--
<?php

$base = new Uri\Rfc3986\Uri('https://oldUser:oldPass@example.com:123/base/path?oldQuery#oldFragment');

$uri = new Uri\Rfc3986\UriBuilder()
    ->setHost('')
    ->setPort(124)
    ->build($base);

var_dump($uri->toRawString());
var_dump($uri);
var_dump($uri->equals(new Uri\Rfc3986\Uri($uri->toRawString())));
var_dump($uri->equals(new Uri\Rfc3986\Uri('//:124', $base), Uri\UriComparisonMode::IncludeFragment));

?>
--EXPECTF--
string(12) "https://:124"
object(Uri\Rfc3986\Uri)#%d (8) {
  ["scheme"]=>
  string(5) "https"
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(0) ""
  ["port"]=>
  int(124)
  ["path"]=>
  string(0) ""
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
bool(true)
bool(true)
