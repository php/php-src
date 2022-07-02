--TEST--
Bug #70219 Use after free vulnerability in session deserializer
--EXTENSIONS--
session
--FILE--
<?php
ini_set('session.serialize_handler', 'php_serialize');
session_start();

class obj implements Serializable {
    var $data;
    function serialize() {
        return serialize($this->data);
    }
    function unserialize($data) {
        session_decode($data);
    }
}

$inner = 'r:2;';
$exploit = 'a:2:{i:0;C:3:"obj":'.strlen($inner).':{'.$inner.'}i:1;C:3:"obj":'.strlen($inner).':{'.$inner.'}}';

$data = unserialize($exploit);

for ($i = 0; $i < 5; $i++) {
    $v[$i] = 'hi'.$i;
}

var_dump($data);
var_dump($_SESSION);
?>
--EXPECTF--
Deprecated: %s implements the Serializable interface, which is deprecated. Implement __serialize() and __unserialize() instead (or in addition, if support for old PHP versions is necessary) in %s on line %d
array(2) {
  [0]=>
  object(obj)#%d (1) {
    ["data"]=>
    NULL
  }
  [1]=>
  object(obj)#%d (1) {
    ["data"]=>
    NULL
  }
}
object(obj)#1 (1) {
  ["data"]=>
  NULL
}
