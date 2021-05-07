--TEST--
Cannot have implicit set without get
--FILE--
<?php

class Test {
    public $prop { set; }
}

?>
--EXPECTF--
Fatal error: Cannot have implicit set without get in %s on line %d
