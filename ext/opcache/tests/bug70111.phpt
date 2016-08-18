--TEST--
Bug #70111 (Segfault when a function uses both an explicit return type and an explicit cast)
--INI--
opcache.enable=1
opcache.enable_cli=1
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

var_dump(foo());

function foo() : string {
  return (string) 42;
}
?>
--EXPECT--
string(2) "42"
