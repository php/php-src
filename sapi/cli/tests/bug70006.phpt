--TEST--
Bug #70006 (cli - function with default arg = STDOUT crash output)
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php

function foo1($stream = STDOUT)
{
  //do nothing
}
foo1();
?>
okey
--EXPECT--
okey
