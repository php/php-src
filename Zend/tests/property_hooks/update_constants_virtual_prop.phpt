--TEST--
Make sure constant updating works in the presence of virtual properties
--FILE--
<?php

class Test {
    public $prop { get {} }
    public $prop2 = FOO;
}
define('FOO', 42);
$test = new Test;
var_dump($test->prop2);

?>
--EXPECT--
int(42)
