--TEST--
Test empty ports become null
--EXTENSIONS--
uri
--FILE--
<?php

$uri = new \Uri\Rfc3986\Uri('https://example.com:');
var_dump($uri, $uri->getPort());

?>
--EXPECTF--
object(Uri\Rfc3986\Uri)#%d (8) {
  ["scheme"]=>
  string(5) "https"
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(11) "example.com"
  ["port"]=>
  NULL
  ["path"]=>
  string(0) ""
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
NULL
