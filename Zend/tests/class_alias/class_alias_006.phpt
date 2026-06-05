--TEST--
Testing creation of alias to an internal class
--FILE--
<?php

class_alias('stdClass', 'foo');
$foo = new foo();
var_dump($foo);

?>
--EXPECT--
object(stdClass)#1 (0) {
}
