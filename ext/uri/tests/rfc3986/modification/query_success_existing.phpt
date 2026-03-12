--TEST--
Test Uri\Rfc3986\Uri component modification - query - changing an existing one
--EXTENSIONS--
uri
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse("https://example.com?foo=bar");
$uri2 = $uri1->withQuery("foo=bar&baz=qux");

var_dump($uri1->getRawQuery());
var_dump($uri2->getRawQuery());
var_dump($uri2->toRawString());
var_dump($uri2->getQuery());
var_dump($uri2->toString());

?>
--EXPECT--
string(7) "foo=bar"
string(15) "foo=bar&baz=qux"
string(35) "https://example.com?foo=bar&baz=qux"
string(15) "foo=bar&baz=qux"
string(35) "https://example.com?foo=bar&baz=qux"
