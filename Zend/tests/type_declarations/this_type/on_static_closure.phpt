--TEST--
$this on static closure
--FILE--
<?php

class Test {
    public function method() {
        return static function(): $this {};
    }
}

?>
--EXPECTF--
Fatal error: Cannot use "$this" type on static closure in %s on line %d
