--TEST--
Test recomposition of URIs
--EXTENSIONS--
uri
--FILE--
<?php

var_dump(Uri\Rfc3986Uri::parse("http://example.com?foo=Hell%C3%B3+W%C3%B6rld")->__toString());

var_dump(Uri\WhatWgUri::parse("http://example.com?foo=Hell%C3%B3+W%C3%B6rld")->__toString());

?>
--EXPECT--
string(44) "http://example.com?foo=Hell%C3%B3+W%C3%B6rld"
string(45) "http://example.com/?foo=Hell%C3%B3+W%C3%B6rld"
