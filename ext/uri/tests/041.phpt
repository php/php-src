--TEST--
Test relative URI parsing
--EXTENSIONS--
uri
--FILE--
<?php

$uri = Uri\Rfc3986Uri::parse("?query#fragment");
var_dump($uri);

$errors = [];
$uri = Uri\WhatWgUri::parse("?query#fragment", null, $errors);
var_dump($uri);
var_dump($errors);

?>
--EXPECTF--
object(Uri\Rfc3986Uri)#%d (%d) {
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
