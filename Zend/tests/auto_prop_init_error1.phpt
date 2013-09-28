--TEST--
Automatic property initialization: $this-> prefix error
--FILE--
<?php

class Test {
    public function __construct($foo->bar) {}
}

?>
--EXPECTF--
Fatal error: Property name can only be prefixed by $this-> in %s on line %d
