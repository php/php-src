--TEST--
Private property inheritance check
--FILE--
<?php
Class A {
    private $c;
}

Class B extends A {
    private $c;
}

Class C extends B {
}

var_dump(new C);
?>
--EXPECTF--
object(C)#%d (2) {
  ["c":"A":private]=>
  NULL
  ["c":"B":private]=>
  NULL
}
