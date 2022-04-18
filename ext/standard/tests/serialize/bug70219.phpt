--TEST--
Bug #70219 Use after free vulnerability in session deserializer
--EXTENSIONS--
session
--INI--
error_reporting=E_ALL&~E_DEPRECATED
--FILE--
<?php
class obj implements Serializable {
    var $data;
    function serialize() {
        return serialize($this->data);
    }
    function unserialize($data) {
        session_start();
        session_decode($data);
    }
}

$inner = 'ryat|a:1:{i:0;a:1:{i:1;';
$exploit = 'a:2:{i:0;C:3:"obj":'.strlen($inner).':{'.$inner.'}i:1;R:4;}';

$data = unserialize($exploit);

for ($i = 0; $i < 5; $i++) {
    $v[$i] = 'hi'.$i;
}

var_dump($data);
?>
--EXPECTF--
Warning: session_decode(): Failed to decode session object. Session has been destroyed in %s on line %d

Notice: unserialize(): Error at offset 55 of 56 bytes in %s on line %d
bool(false)
