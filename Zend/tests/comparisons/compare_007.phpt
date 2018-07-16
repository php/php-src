--TEST--
comparing objects of different types
--FILE--
<?php
var_dump(new stdClass()  < new DateTime());  // false
var_dump(new stdClass()  > new DateTime());  // false
var_dump(new stdClass() == new DateTime());  // false

var_dump(new DateTime()  < new stdClass());  // false
var_dump(new DateTime()  > new stdClass());  // false
var_dump(new DateTime() == new stdClass());  // false

var_dump(new stdClass() <=> new DateTime()); // 1  
var_dump(new DateTime() <=> new stdClass()); // 1  
?>
--EXPECTF-- 
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
int(1)
int(1)
