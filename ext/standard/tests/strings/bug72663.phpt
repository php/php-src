--TEST--
Bug #72663: Create an Unexpected Object and Don't Invoke __wakeup() in Deserialization
--FILE--
<?php
class obj implements Serializable {
    var $data;
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
DONE
--EXPECTF--
Deprecated: %s implements the Serializable interface, which is deprecated. Implement __serialize() and __unserialize() instead (or in addition, if support for old PHP versions is necessary) in %s on line %d

Notice: unserialize(): Unexpected end of serialized data in %sbug72663.php on line %d

Notice: unserialize(): Error at offset 49 of 50 bytes in %sbug72663.php on line %d

Notice: unserialize(): Error at offset 82 of 83 bytes in %sbug72663.php on line %d
bool(false)
DONE
