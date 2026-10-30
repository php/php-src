--TEST--
GH-17200: Incorrect dynamic property offset
--FILE--
<?php

class A {
    public $prop;
}

class B extends A {
    public $prop {
        get => $this->prop;
    }
}

$b = new B();
var_dump($b);
echo json_encode($b), "\n";

?>
--EXPECTF--
object(B)#%d (1) {
  ["prop"]=>
  NULL
}
{"prop":null}
