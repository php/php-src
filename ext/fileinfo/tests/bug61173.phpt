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
Warning: finfo::finfo() expects at most 2 parameters, 3 given in %s on line %d
NULL
