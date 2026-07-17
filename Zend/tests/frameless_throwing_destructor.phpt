--TEST--
Frameless call with throwing destructor
--FILE--
<?php
class Foo {}
class Bar {
    public function __destruct() {
        throw new Exception();
    }
}
in_array(new Foo(), [new Bar()], true);
?>
--EXPECTF--
Fatal error: Uncaught Exception in %s:%d
Stack trace:
#0 [internal function]: Bar->__destruct()
#1 {main}
  thrown in %s on line %d
