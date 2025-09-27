--TEST--
Test Uri\Rfc3986\Uri component modification - host - URL encoded characters
--EXTENSIONS--
uri
--FILE--
<?php

$uri1 = Uri\Rfc3986\Uri::parse("https://example.com");
$uri2 = $uri1->withHost("t%65st.com"); // test.com

var_dump($uri1->getRawHost());
var_dump($uri2->getRawHost());
var_dump($uri2->getHost());

?>
--EXPECT--
string(11) "example.com"
string(10) "t%65st.com"
string(8) "test.com"
