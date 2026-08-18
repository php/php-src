--TEST--
Test Uri\Rfc3986\UriBuilder::build() with shared property values
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

// make fragment share the path value.
$serialised = str_replace(
    "s:32:\"{$scope}fragment\";N;",
    "s:32:\"{$scope}fragment\";R:6;",
    $serialised,
);

var_dump(unserialize($serialised)->build()->toRawString());

?>
--EXPECT--
string(30) "https://example.com/path#/path"
