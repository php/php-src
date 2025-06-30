--TEST--
Test percent-decoding of reserved characters in the path
--EXTENSIONS--
uri
--FILE--
<?php

$url = new Uri\Whatwg\Url("https://example.com/foo/bar%2Fbaz");

var_dump($url->toAsciiString());
var_dump($url->getPath());

?>
--EXPECT--
string(33) "https://example.com/foo/bar%2Fbaz"
string(14) "/foo/bar%2Fbaz"
