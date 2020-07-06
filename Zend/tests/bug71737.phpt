--TEST--
Bug #71737: Memory leak in closure with parameter named $this
--FILE--
<?php

class Test {
    public function method() {
        return function($this) {};
    }
}

(new Test)->method()(new stdClass);

?>
--EXPECTF--
Fatal error: {closure}(): Parameter #1 cannot be called $this in %s on line %d
