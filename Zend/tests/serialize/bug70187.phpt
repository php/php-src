--TEST--
Bug #70187 (Notice: unserialize(): Unexpected end of serialized data)
--FILE--
<?php
class A {
    public $b;
}

$a = new A;
var_dump($a); // force properties HT
unset($a->b);
var_dump(serialize($a));
?>
--EXPECT--
object(A)#1 (1) {
  ["b"]=>
  NULL
}
string(12) "O:1:"A":0:{}"
