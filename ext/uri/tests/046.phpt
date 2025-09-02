--TEST--
Test special path variants
--EXTENSIONS--
uri
--FILE--
<?php

$uri = new Uri\Rfc3986\Uri("mailto:johndoe@example.com");

var_dump($uri->toRawString());
var_dump($uri->getPath());
var_dump($uri->getRawPath());

$uri = new Uri\Rfc3986\Uri("foo/bar");
var_dump($uri->toRawString());
var_dump($uri->getPath());
var_dump($uri->getRawPath());

$uri = new Uri\Rfc3986\Uri("/foo/bar");
var_dump($uri->toRawString());
var_dump($uri->getPath());
var_dump($uri->getRawPath());

$uri = new Uri\Rfc3986\Uri("//");
var_dump($uri->toRawString());
var_dump($uri->getPath());
var_dump($uri->getRawPath());

$uri = new Uri\Rfc3986\Uri("///");
var_dump($uri->toRawString());
var_dump($uri->getPath());
var_dump($uri->getRawPath());

$uri = new Uri\Rfc3986\Uri("https://example.com");

var_dump($uri->toRawString());
var_dump($uri->getPath());
var_dump($uri->getRawPath());

$uri = new Uri\Rfc3986\Uri("https://example.com/");

var_dump($uri->toRawString());
var_dump($uri->getPath());
var_dump($uri->getRawPath());

$url = new Uri\Whatwg\Url("mailto:johndoe@example.com");

var_dump($url->toAsciiString());
var_dump($url->getPath());

$url = new Uri\Whatwg\Url("https://example.com");

var_dump($url->toAsciiString());
var_dump($url->getPath());

$url = new Uri\Whatwg\Url("https://example.com/");

var_dump($url->toAsciiString());
var_dump($url->getPath());

?>
--EXPECT--
string(26) "mailto:johndoe@example.com"
string(19) "johndoe@example.com"
string(19) "johndoe@example.com"
string(7) "foo/bar"
string(7) "foo/bar"
string(7) "foo/bar"
string(8) "/foo/bar"
string(8) "/foo/bar"
string(8) "/foo/bar"
string(2) "//"
string(0) ""
string(0) ""
string(3) "///"
string(1) "/"
string(1) "/"
string(19) "https://example.com"
string(0) ""
string(0) ""
string(20) "https://example.com/"
string(1) "/"
string(1) "/"
string(26) "mailto:johndoe@example.com"
string(19) "johndoe@example.com"
string(20) "https://example.com/"
string(1) "/"
string(20) "https://example.com/"
string(1) "/"
