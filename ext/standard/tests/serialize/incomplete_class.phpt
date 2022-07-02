--TEST--
(un)serializing __PHP_Incomplete_Class instance
--FILE--
<?php

$d = serialize(new __PHP_Incomplete_Class);
$o = unserialize($d);
var_dump($o);

try {
    $o->test = "a";
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
var_dump($o->test);
var_dump($o->test2);

echo "Done\n";
?>
--EXPECTF--
object(__PHP_Incomplete_Class)#%d (0) {
}
The script tried to modify a property on an incomplete object. Please ensure that the class definition "unknown" of the object you are trying to operate on was loaded _before_ unserialize() gets called or provide an autoloader to load the class definition

Warning: main(): The script tried to access a property on an incomplete object. Please ensure that the class definition "unknown" of the object you are trying to operate on was loaded _before_ unserialize() gets called or provide an autoloader to load the class definition in %s on line %d
NULL

Warning: main(): The script tried to access a property on an incomplete object. Please ensure that the class definition "unknown" of the object you are trying to operate on was loaded _before_ unserialize() gets called or provide an autoloader to load the class definition in %s on line %d
NULL
Done
