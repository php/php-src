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
Fatal error: Interface RecurisiveFooFar cannot implement itself in %sbug30922.php on line %d
