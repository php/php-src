--TEST--
Cannot use hooks for static property (for now)
--FILE--
<?php

class Test {
    public static $prop { get; set; }
}

?>
--EXPECTF--
Fatal error: Cannot declare hooks for static property in %s on line %d
