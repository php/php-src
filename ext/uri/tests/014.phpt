--TEST--
Test recomposition of URIs
--EXTENSIONS--
uri
--FILE--
<?php

var_dump(Uri\Rfc3986\Uri::parse("http://example.com?foo=Hell%C3%B3+W%C3%B6rld")->toRawString());

var_dump(Uri\WhatWg\Url::parse("http://example.com?foo=Hell%C3%B3+W%C3%B6rld")->toAsciiString());

?>
--EXPECT--
string(44) "http://example.com?foo=Hell%C3%B3+W%C3%B6rld"
string(45) "http://example.com/?foo=Hell%C3%B3+W%C3%B6rld"
