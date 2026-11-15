--TEST--
Anonymous class name is not truncated in method inheritance errors
--FILE--
<?php
class A { public function f() {} }
new class extends A { private function f() {} };
?>
--EXPECTF--
Fatal error: Access level to A@anonymous%0%s::f() must be public (as in class A) in %s on line %d
