--TEST--
Bug #64354 (Unserialize array of objects whose class can't be autoloaded fail)
--FILE--
<?php
class A {
    public function __sleep() {
        throw new Exception("Failed");
    }
}

class B implements Serializable {
    public function serialize() {
        return NULL;
    }

    public function unserialize($data) {
    }
}

$data = array(new A, new B);

try {
    serialize($data);
} catch (Exception $e) {
    var_dump($e->getMessage());
}
?>
--EXPECTF--
Deprecated: %s implements the Serializable interface, which is deprecated. Implement __serialize() and __unserialize() instead (or in addition, if support for old PHP versions is necessary) in %s on line %d
string(6) "Failed"
