--TEST--
GH-15907: Failed assertion when promoting inheritance error to exception
--FILE--
<?php

set_error_handler(function($errno, $msg) {
    throw new Exception($msg);
});

class C implements Serializable {
    public function serialize() {}
    public function unserialize($serialized) {}
}

?>
--EXPECTF--
Fatal error: Uncaught Exception: C implements the Serializable interface, which is deprecated. Implement __serialize() and __unserialize() instead (or in addition, if support for old PHP versions is necessary) in %s:%d
Stack trace:
#0 %s(%d): {closure:%s:%d}(8192, 'C implements th...', '%s', 7)
#1 {main}
  thrown in %s on line %d
