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
Fatal error: During inheritance of C, while implementing Serializable: Uncaught Exception: C implements the Serializable interface, which is deprecated. Implement __serialize() and __unserialize() instead (or in addition, if support for old PHP versions is necessary) in %s:%d
%a
