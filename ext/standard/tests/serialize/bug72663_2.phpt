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

$inner = 'a:1:{i:0;O:9:"Exception":2:{s:7:"'."\0".'*'."\0".'file";R:4;}';
$exploit = 'a:2:{i:0;C:3:"obj":'.strlen($inner).':{'.$inner.'}i:1;R:4;}';
var_dump(unserialize($exploit));

?>
--EXPECTF--
Notice: unserialize(): Unexpected end of serialized data in %s on line %d

Notice: unserialize(): Error at offset 46 of 47 bytes in %s on line %d

Notice: unserialize(): Error at offset 79 of 80 bytes in %s on line %d
bool(false)
