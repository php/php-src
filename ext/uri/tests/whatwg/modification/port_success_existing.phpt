--TEST--
Test Uri\WhatWg\Url component modification - port - changing an existing one
--EXTENSIONS--
uri
--FILE--
<?php

$url1 = Uri\WhatWg\Url::parse("https://example.com:80");
$url2 = $url1->withPort(433);

var_dump($url1->getPort());
var_dump($url2->getPort());

?>
--EXPECT--
int(80)
int(433)
