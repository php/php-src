--TEST--
Checking if exists interface, abstract and final class
--FILE--
<?php

interface a { }

abstract class b { }

final class c { }

var_dump(class_exists('a'));
var_dump(class_exists('b'));
var_dump(class_exists('c'));

?>
--EXPECT--
bool(false)
bool(true)
bool(true)
