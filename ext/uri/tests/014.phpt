--TEST--
Test recomposition of URIs
--EXTENSIONS--
uri
--FILE--
<?php

var_dump(\Uri\Uri::fromRfc3986("http://example.com?foo=Hell%C3%B3+W%C3%B6rld")->__toString());

var_dump(\Uri\Uri::fromWhatWg("http://example.com?foo=Hell%C3%B3+W%C3%B6rld")->__toString());

?>
--EXPECTF--
string(44) "http://example.com?foo=Hell%C3%B3+W%C3%B6rld"
string(45) "http://example.com/?foo=Hell%C3%B3+W%C3%B6rld"
