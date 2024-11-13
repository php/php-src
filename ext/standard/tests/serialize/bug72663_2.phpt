--TEST--
Bug #72663 (2): Don't allow references into failed unserialize
--FILE--
<?php

class obj implements Serializable {
    public $data;
    function serialize() {
        return serialize($this->data);
    }
    function unserialize($data) {
        $this->data = unserialize($data);
    }
}

$inner = 'a:1:{i:0;O:9:"Exception":2:{s:7:"'."\0".'*'."\0".'file";s:0:"";}';
$exploit = 'a:2:{i:0;C:3:"obj":'.strlen($inner).':{'.$inner.'}i:1;R:4;}';
var_dump(unserialize($exploit));

?>
--EXPECTF--
Deprecated: %s implements the Serializable interface, which is deprecated. Implement __serialize() and __unserialize() instead (or in addition, if support for old PHP versions is necessary) in %s on line %d

Warning: unserialize(): Unexpected end of serialized data in %s on line %d

Warning: unserialize(): Error at offset 49 of 50 bytes in %s on line %d

Warning: unserialize(): Error at offset 82 of 83 bytes in %s on line %d
bool(false)
