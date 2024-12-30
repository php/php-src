--TEST--
Test relative URI parsing
--EXTENSIONS--
uri
--XFAIL--
Fails due to the errors array
--FILE--
<?php

$uri = Uri\Rfc3986\Uri::parse("?query#fragment");
var_dump($uri);

$errors = [];
$uri = Uri\WhatWg\Url::parse("?query#fragment", null, $errors);
var_dump($uri);
var_dump($errors);

?>
--EXPECTF--
object(Uri\Rfc3986\Uri)#%d (%d) {
  ["scheme"]=>
  NULL
  ["user"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  NULL
  ["port"]=>
  NULL
  ["path"]=>
  NULL
  ["query"]=>
  string(5) "query"
  ["fragment"]=>
  string(8) "fragment"
}
NULL
