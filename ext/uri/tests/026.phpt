--TEST--
Test property mutation - host
--EXTENSIONS--
uri
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("https://example.com");
$url2 = $url1->withHost("test.com");
$url3 = $url2->withHost("t%65st.com"); // test.com
try {
    $url3->withHost("test.com:8080");
} catch (Uri\WhatWg\InvalidUrlException $e) {
    echo $e->getMessage() . "\n";
}

var_dump($url1->getAsciiHost());
var_dump($url2->getAsciiHost());
var_dump($url3->getAsciiHost());

try {
    $url3->withHost("t%3As%2Ft.com"); // t:s/t.com
} catch (Uri\WhatWg\InvalidUrlException $e) {
    echo $e->getMessage() . "\n";
}

try {
    $url3->withHost("t:s/t.com");     // t:s/t.com
} catch (Uri\WhatWg\InvalidUrlException $e) {
    echo $e->getMessage() . "\n";
}

try {
    $url2->withHost(null);
} catch (Uri\WhatWg\InvalidUrlException $e) {
    echo $e->getMessage() . "\n";
}

$url1 = Uri\WhatWg\Url::parse("ftp://foo.com?query=abc#foo");
$url2 = $url1->withHost("test.com");

var_dump($url1->getAsciiHost());
var_dump($url2->getAsciiHost());

?>
--EXPECTF--
The specified host is malformed
string(11) "example.com"
string(8) "test.com"
string(8) "test.com"
The specified host is malformed (DomainInvalidCodePoint)
The specified host is malformed
The specified host is malformed (HostMissing)
string(7) "foo.com"
string(8) "test.com"
