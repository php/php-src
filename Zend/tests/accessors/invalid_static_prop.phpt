--TEST--
Cannot use accessors for static property (for now)
--FILE--
<?php

class Test {
    public static $prop { get; set; }
}

?>
--EXPECTF--
Fatal error: Cannot declare accessors for static property in %s on line %d
