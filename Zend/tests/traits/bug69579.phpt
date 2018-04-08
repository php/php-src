--TEST--
Bug #69579 (Internal trait double-free)
--SKIPIF--
<?php
if (!trait_exists('_ZendTestTrait')) die("skip requires internal trait");
?>
--FILE--
<?php

class Bar{
  use _ZendTestTrait;
}

$bar = new Bar();
var_dump($bar->testMethod());
// destruction causes a double-free and explodes

?>
--EXPECT--
bool(true)
