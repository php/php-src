--TEST--
bug #71428: Validation type inheritance with = NULL
--XFAIL--
This is a BC break
--FILE--
<?php
class A { }
class B           {  public function m(A $a = NULL, $n) { echo "B.m";} };
class C extends B {  public function m(A $a       , $n) { echo "C.m";} };
?>
--EXPECTF--
Warning: Declaration of C::m(A $a, $n) should be compatible with B::m(A $a = NULL, $n) in %sbug71428.3.php on line 4

