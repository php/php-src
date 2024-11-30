--TEST--
Test URN parsing
--EXTENSIONS--
uri
--FILE--
<?php

$uri = Uri\Rfc3986Uri::parse("urn:uuid:6e8bc430-9c3a-11d9-9669-0800200c9a66");
var_dump($uri);

$uri = Uri\WhatWgUri::parse("urn:uuid:6e8bc430-9c3a-11d9-9669-0800200c9a66");
var_dump($uri);

?>
--EXPECTF--
object(Uri\Rfc3986Uri)#%d (%d) {
  ["scheme"]=>
  string(3) "urn"
  ["user"]=>
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
object(Uri\WhatWgUri)#%d (%d) {
  ["scheme"]=>
  string(3) "urn"
  ["user"]=>
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
