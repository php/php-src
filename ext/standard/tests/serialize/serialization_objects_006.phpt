--TEST--
Behaviour of incomplete class is preserved even when it was not created by unserialize().
--FILE--
<?php
$a = unserialize('O:7:"Unknown":0:{}');
var_dump($a);
var_dump($a->p);

echo "Done";
?>
--EXPECTF--
object(__PHP_Incomplete_Class)#1 (1) {
  ["__PHP_Incomplete_Class_Name"]=>
  string(7) "Unknown"
}

Warning: main(): The script tried to access a property on an incomplete object. Please ensure that the class definition "Unknown" of the object you are trying to operate on was loaded _before_ unserialize() gets called or provide an autoloader to load the class definition in %s on line %d
NULL
Done
