--TEST--
Behaviour of incomplete class is preserved even when it was not created by unserialize().
--FILE--
<?php
$a = new __PHP_Incomplete_Class;
var_dump($a);
var_dump($a->p);

echo "Done";
?>
--EXPECTF--
object(__PHP_Incomplete_Class)#%d (0) {
}

Notice: main(): The script tried to execute a method or access a property of an incomplete object. Please ensure that the class definition "unknown" of the object you are trying to operate on was loaded _before_ unserialize() gets called or provide an autoloader to load the class definition in %s on line %d
NULL
Done
