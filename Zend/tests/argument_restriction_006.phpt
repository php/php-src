--TEST--
Bug #60174 (Notice when array in method prototype error)
--FILE--
<?php
Abstract Class Base {
    public function test($foo, $extra = array("test")) {
    }
}

class Sub extends Base {
    public function test($foo, $extra) {
    }
}
?>
--EXPECTF--
Fatal error: Declaration of Sub::test($foo, $extra) must be compatible with Base::test($foo, $extra = [...]) in %s on line %d
