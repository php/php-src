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
Deprecated: The Serializable interface is deprecated. If you need to retain the Serializable interface for cross-version compatibility, you can suppress this warning by implementing __serialize() and __unserialize() in addition, which will take precedence over Serializable in PHP versions that support them in %s on line %d
string(6) "Failed"
