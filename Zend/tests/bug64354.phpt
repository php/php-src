--TEST--
Bug #64354 (Unserialize array of objects whose class can't be autoloaded fail)
--FILE--
<?php
class B implements Serializable {
    public function serialize() {
        throw new Exception("serialize");
        return NULL;
    }

    public function unserialize($data) {
    }
}

$data = array(new B);

try {
    serialize($data);
} catch (Exception $e) {
    var_dump($e->getMessage());
}
?>
--EXPECTF--
string(9) "serialize"
