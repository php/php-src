--TEST--
Handling of UNDEF property in compound assign 
--FILE--
<?php
class C {
    public $a = 0;
}
function foo() {
    $x = new C;
    $x->a = 1;
    unset($x->a);
    $x->a += 2;
    var_dump($x);
}
foo();
?>
--EXPECTF--
Notice: Undefined property: C::$a in %s on line %d
object(C)#1 (1) {
  ["a"]=>
  int(2)
}
