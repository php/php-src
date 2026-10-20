--TEST--
Test Uri\Rfc3986\UriBuilder::setPath() - success - contains a colon in the first segment when the scheme is present
--FILE--
<?php

$builder = new Uri\Rfc3986\UriBuilder();
$builder->setScheme("https");
$builder->setPath(":foo/bar/baz");
$uri = $builder->build();

var_dump($uri->toRawString());
var_dump($uri);
var_dump($uri->equals(new Uri\Rfc3986\Uri($uri->toRawString())));

?>
--EXPECTF--
string(%d) "https::foo/bar/baz"
object(Uri\Rfc3986\Uri)#%d (%d) {
  ["scheme"]=>
  string(5) "https"
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  NULL
  ["port"]=>
  NULL
  ["path"]=>
  string(12) ":foo/bar/baz"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
bool(true)
