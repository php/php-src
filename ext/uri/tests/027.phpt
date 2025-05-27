--TEST--
Test property mutation - port
--EXTENSIONS--
uri
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("https://example.com:8080");
$url2 = $url1->withPort(22);
$url3 = $url2->withPort(null);

var_dump($url1->getPort());
var_dump($url2->getPort());
var_dump($url3->getPort());

$url1 = Uri\WhatWg\Url::parse("ftp://foo.com:443?query=abc#foo");
$url2 = $url1->withPort(8080);

var_dump($url1->getPort());
var_dump($url2->getPort());

$url1 = Uri\WhatWg\Url::parse("file:///foo/bar");
$url2 = $url1->withPort(80);

var_dump($url1->getPort());
var_dump($url2->getPort());

?>
--EXPECT--
int(8080)
int(22)
NULL
int(443)
int(8080)
NULL
NULL
