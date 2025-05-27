--TEST--
Test parsing of various schemes
--EXTENSIONS--
uri
--FILE--
<?php

var_dump(Uri\WhatWg\Url::parse("mailto:Joe@Example.COM"));

var_dump(Uri\WhatWg\Url::parse("file:///E:\\Documents and Settings"));

?>
--EXPECTF--
object(Uri\WhatWg\Url)#%d (%d) {
  ["scheme"]=>
  string(6) "mailto"
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  NULL
  ["port"]=>
  NULL
  ["path"]=>
  string(15) "Joe@Example.COM"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
object(Uri\WhatWg\Url)#%d (%d) {
  ["scheme"]=>
  string(4) "file"
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  NULL
  ["port"]=>
  NULL
  ["path"]=>
  string(30) "/E:/Documents%20and%20Settings"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
