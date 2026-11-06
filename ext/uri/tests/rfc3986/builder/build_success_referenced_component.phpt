--TEST--
Test Uri\Rfc3986\UriBuilder::build() - success - referenced component
--FILE--
<?php

$scope = "\0Uri\\Rfc3986\\UriBuilder\0";

$serialised = serialize(new Uri\Rfc3986\UriBuilder());

$serialised = str_replace(
    "s:30:\"{$scope}scheme\";N;",
    "s:30:\"{$scope}scheme\";s:5:\"https\";",
    $serialised,
);

$serialised = str_replace(
    "s:28:\"{$scope}host\";N;",
    "s:28:\"{$scope}host\";s:11:\"example.com\";",
    $serialised,
);

$serialised = str_replace(
    "s:28:\"{$scope}path\";s:0:\"\";",
    "s:28:\"{$scope}path\";s:5:\"/path\";",
    $serialised,
);

// make fragment share path value
$serialised = str_replace(
    "s:32:\"{$scope}fragment\";N;",
    "s:32:\"{$scope}fragment\";R:6;",
    $serialised,
);

$uri = unserialize($serialised)->build();

var_dump($uri->toRawString());
var_dump($uri);
var_dump($uri->equals(new Uri\Rfc3986\Uri($uri->toRawString())));

?>
--EXPECTF--
string(30) "https://example.com/path#/path"
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
  string(5) "/path"
  ["query"]=>
  NULL
  ["fragment"]=>
  string(5) "/path"
}
bool(true)
