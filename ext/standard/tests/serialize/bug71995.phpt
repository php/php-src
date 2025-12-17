--TEST--
Bug #71995 (Returning the same var twice from __sleep() produces broken serialized data)
--FILE--
<?php

class A {
    public $b;
    public function __construct() {
        $this->b = new StdClass();
    }
    public  function __sleep() {
        return array("b", "b");
    }
}
$a = new A();
$s = serialize($a);
var_dump($s);
var_dump(unserialize($s));
?>
--EXPECTF--
Warning: serialize(): "b" is returned from __sleep() multiple times in %s on line %d
string(39) "O:1:"A":1:{s:1:"b";O:8:"stdClass":0:{}}"
object(A)#%d (1) {
  ["b"]=>
  object(stdClass)#%d (0) {
  }
}
