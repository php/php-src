--TEST--
Test Uri\WhatWg\Url component modification - port - changing an existing one
--EXTENSIONS--
uri
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("scheme://example.com:80");
$url2 = $url1->withPort(443);

var_dump($url1->getPort());
var_dump($url2->getPort());
var_dump($url2->toAsciiString());

?>
--EXPECT--
int(80)
int(443)
string(24) "scheme://example.com:443"
