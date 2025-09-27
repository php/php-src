--TEST--
Test Uri\WhatWg\Url component modification - host - URL encoded characters
--EXTENSIONS--
uri
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("https://example.com");
$url2 = $url1->withHost("t%65st.com"); // test.com

var_dump($url1->getAsciiHost());
var_dump($url2->getAsciiHost());

?>
--EXPECT--
string(11) "example.com"
string(8) "test.com"
