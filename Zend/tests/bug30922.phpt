--TEST--
Bug #30922 (reflective functions crash PHP when interfaces extend themselves)
--FILE--
<?php
interface RecurisiveFooFar extends RecurisiveFooFar {}
class A implements RecurisiveFooFar {}

$a = new A();
var_dump($a instanceOf A);
echo "ok\n";
?>
--EXPECTF--
Fatal error: Uncaught Error: Interface "RecurisiveFooFar" not found in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
