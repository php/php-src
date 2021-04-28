--TEST--
Bug #70219 Use after free vulnerability in session deserializer
--SKIPIF--
<?php
if (!extension_loaded('session')) die('skip session extension not available');
?>
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
Deprecated: The Serializable interface is deprecated. Implement __serialize() and __unserialize() instead (or in addition, if support for old PHP versions is necessary) in %s on line %d

Warning: session_start(): Session cannot be started after headers have already been sent in %s on line %d

Warning: session_decode(): Session data cannot be decoded when there is no active session in %s on line %d

Notice: unserialize(): Error at offset 55 of 56 bytes in %s on line %d
bool(false)
