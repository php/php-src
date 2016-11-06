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

$inner = 'a:1:{i:0;O:9:"Exception":2:{s:7:"'."\0".'*'."\0".'file";R:4;}';
$exploit = 'a:2:{i:0;C:3:"obj":'.strlen($inner).':{'.$inner.'}i:1;R:4;}';

$data = unserialize($exploit);
echo $data[1];
?>
DONE
--EXPECTF--
Notice: unserialize(): Unexpected end of serialized data in %sbug72663.php on line %d

Notice: unserialize(): Error at offset 46 of 47 bytes in %sbug72663.php on line %d

Notice: unserialize(): Error at offset 79 of 80 bytes in %sbug72663.php on line %d
DONE
