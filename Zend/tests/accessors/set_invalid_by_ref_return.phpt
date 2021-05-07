--TEST--
set accessor cannot return by ref
--FILE--
<?php

class Test {
    public $prop { &set; }
}

?>
--EXPECTF--
Fatal error: Accessor "set" cannot return by reference in %s on line %d
