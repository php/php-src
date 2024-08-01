--TEST--
Test encoding and normalization
--EXTENSIONS--
uri
--FILE--
<?php

var_dump(\Uri\Uri::fromRfc3986("HttPs://0300.0250.0000.0001/path?query=foo%20bar"));

var_dump(\Uri\Uri::fromWhatWg("HttPs://0300.0250.0000.0001/path?query=foo%20bar"));

?>
--EXPECTF--
object(Uri\Uri)#%d (%d) {
  ["scheme"]=>
  string(5) "HttPs"
  ["host"]=>
  string(19) "0300.0250.0000.0001"
  ["port"]=>
  NULL
  ["user"]=>
  NULL
  ["password"]=>
  NULL
  ["path"]=>
  string(4) "path"
  ["query"]=>
  string(15) "query=foo%20bar"
  ["fragment"]=>
  NULL
}
object(Uri\Uri)#%d (%d) {
  ["scheme"]=>
  string(5) "https"
  ["host"]=>
  string(11) "1.0.168.192"
  ["port"]=>
  NULL
  ["user"]=>
  NULL
  ["password"]=>
  NULL
  ["path"]=>
  string(4) "path"
  ["query"]=>
  string(15) "query=foo%20bar"
  ["fragment"]=>
  NULL
}
