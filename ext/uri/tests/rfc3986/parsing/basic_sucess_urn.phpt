--TEST--
Test Uri\Rfc3986\Uri parsing - basic - URN
--FILE--
<?php

$uri = Uri\Rfc3986\Uri::parse("urn:uuid:6e8bc430-9c3a-11d9-9669-0800200c9a66");

var_dump($uri);
var_dump($uri->toRawString());
var_dump($uri->toString());

?>
--EXPECTF--
object(Uri\Rfc3986\Uri)#%d (%d) {
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
string(45) "urn:uuid:6e8bc430-9c3a-11d9-9669-0800200c9a66"
string(45) "urn:uuid:6e8bc430-9c3a-11d9-9669-0800200c9a66"
