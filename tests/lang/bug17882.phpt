--TEST--
Bug #17882 (case sensitivity of functions in classes)
--FILE--
<?php
class X {
  function a() {}  
  function A() {} 
}
class Y {
  function A() {}  
  function A() {} 
}
?>
--EXPECTF--
Fatal error: Cannot redeclare a() in %s/bug17882.php on line %d
