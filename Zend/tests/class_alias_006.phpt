--TEST--
Testing creation of alias to an internal class
--FILE--
<?php

class_alias('stdclass', 'foo');

?>
--EXPECTF--
Warning: First argument of class_alias() must be a name of user defined class in %s on line %d
