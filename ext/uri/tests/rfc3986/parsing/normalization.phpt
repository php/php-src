--TEST--
Test Uri\Rfc3986\Uri parsing - normalization
--FILE--
<?php

var_dump(Uri\Rfc3986\Uri::parse("http://////www.EXAMPLE.com:80")->toRawString());
var_dump(Uri\Rfc3986\Uri::parse("https://www.example.com/dir1/../dir2")->toRawString());
var_dump(Uri\Rfc3986\Uri::parse("https://你好你好"));
var_dump(Uri\Rfc3986\Uri::parse("https://０Ｘｃ０．０２５０．０１"));
var_dump(Uri\Rfc3986\Uri::parse("HttPs://0300.0250.0000.0001/path?query=foo%20bar")->toRawString());

?>
--EXPECT--
string(29) "http://////www.EXAMPLE.com:80"
string(36) "https://www.example.com/dir1/../dir2"
NULL
NULL
string(48) "HttPs://0300.0250.0000.0001/path?query=foo%20bar"
