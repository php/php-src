--TEST--
Bug #39017 (foreach(($obj = new myClass) as $v); echo $obj; segfaults)
--FILE--
<?php
class A {}
foreach(($a=(object)new A()) as $v);
var_dump($a); // UNKNOWN:0
?>
--EXPECTF--
object(A)#%d (0) {
}
