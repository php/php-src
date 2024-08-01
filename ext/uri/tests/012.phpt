--TEST--
Test parsing of various schemes
--EXTENSIONS--
uri
--FILE--
<?php

var_dump(\Uri\Uri::fromRfc3986("mailto:Joe@Example.COM"));
var_dump(\Uri\Uri::fromWhatWg("mailto:Joe@Example.COM"));

var_dump(\Uri\Uri::fromRfc3986("file:///E:\\Documents and Settings"));
var_dump(\Uri\Uri::fromWhatWg("file:///E:\\Documents and Settings"));

?>
--EXPECTF--
object(Uri\Uri)#%d (%d) {
  ["scheme"]=>
  string(6) "mailto"
  ["host"]=>
  NULL
  ["port"]=>
  NULL
  ["user"]=>
  NULL
  ["password"]=>
  NULL
  ["path"]=>
  string(15) "Joe@Example.COM"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
object(Uri\Uri)#%d (%d) {
  ["scheme"]=>
  NULL
  ["host"]=>
  NULL
  ["port"]=>
  NULL
  ["user"]=>
  NULL
  ["password"]=>
  NULL
  ["path"]=>
  string(15) "Joe@Example.COM"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
NULL
object(Uri\Uri)#%d (%d) {
  ["scheme"]=>
  string(4) "file"
  ["host"]=>
  NULL
  ["port"]=>
  NULL
  ["user"]=>
  NULL
  ["password"]=>
  NULL
  ["path"]=>
  string(29) "E:/Documents%20and%20Settings"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
