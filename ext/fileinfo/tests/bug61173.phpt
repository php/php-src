--TEST--
Bug #61173: Unable to detect error from finfo constructor
--SKIPIF--
<?php
if (!class_exists('finfo'))
	die('skip no fileinfo extension');
--FILE--
<?php

$finfo = new finfo(1, '', false);
var_dump($finfo);
--EXPECTF--
Fatal error: Uncaught exception 'Exception' with message 'finfo::finfo() expects at most 2 parameters, 3 given' in %sbug61173.php:3
Stack trace:
#0 %sbug61173.php(3): finfo->finfo(1, '', false)
#1 {main}
  thrown in %sbug61173.php on line 3
