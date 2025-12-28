--TEST--
Bug #30922 (reflective functions crash PHP when interfaces extend themselves)
--FILE--
<?php
interface RecursiveFooFar extends RecursiveFooFar {}
class A implements RecursiveFooFar {}

$a = new A();
var_dump($a instanceOf A);
echo "ok\n";
?>
--EXPECTF--
Fatal error: Uncaught Error: Interface "RecursiveFooFar" not found in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
