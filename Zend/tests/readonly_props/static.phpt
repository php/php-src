--TEST--
Readonly static property
--FILE--
<?php

class Test {
    public static readonly int $prop;
}

?>
--EXPECTF--
Fatal error: Static property Test::$prop cannot be readonly in %s on line %d
