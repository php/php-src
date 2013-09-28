--TEST--
Automatic property initialization: Only in constructor error
--FILE--
<?php

class Test {
    public function foo($this->bar) {}
}

?>
--EXPECTF--
Fatal error: Automatic property initialization can only be used in a constructor in %s on line %d
