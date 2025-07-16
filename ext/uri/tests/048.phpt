--TEST--
Test RFC 3986 percent-encoding normalization
--EXTENSIONS--
uri
--FILE--
<?php

$uri = Uri\Rfc3986\Uri::parse("https://%e4%bd%a0%e5%a5%bd%e4%bd%a0%e5%a5%bd.com");         // percent-encoded form of https://你好你好.com
var_dump($uri->toString());

$uri = Uri\Rfc3986\Uri::parse("https://example.com/%e4%bd%a0%e5%a5%bd%e4%bd%a0%e5%a5%bd"); // percent-encoded form of https://example.com/你好你好.com
var_dump($uri->toString());

?>
--EXPECT--
string(48) "https://%E4%BD%A0%E5%A5%BD%E4%BD%A0%E5%A5%BD.com"
string(56) "https://example.com/%E4%BD%A0%E5%A5%BD%E4%BD%A0%E5%A5%BD"
