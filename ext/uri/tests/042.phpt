--TEST--
Test URN parsing
--EXTENSIONS--
uri
--FILE--
<?php

$url = Uri\WhatWg\Url::parse("urn:uuid:6e8bc430-9c3a-11d9-9669-0800200c9a66");
var_dump($url);
?>
--EXPECTF--
object(Uri\WhatWg\Url)#%d (%d) {
  ["scheme"]=>
  string(3) "urn"
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  NULL
  ["port"]=>
  NULL
  ["path"]=>
  string(41) "uuid:6e8bc430-9c3a-11d9-9669-0800200c9a66"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
