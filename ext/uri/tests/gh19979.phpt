--TEST--
GH-19979: Zend/zend_string.h:191:24: runtime error: null pointer passed as argument 2, which is declared to never be null
--FILE--
<?php

$baseUrl = \Uri\WhatWg\Url::parse('https://example.com/path?query');
var_dump(\Uri\WhatWg\Url::parse('relative', $baseUrl));

?>
--EXPECTF--
object(Uri\WhatWg\Url)#%d (8) {
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
  string(9) "/relative"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
