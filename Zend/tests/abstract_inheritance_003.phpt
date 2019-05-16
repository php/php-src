--TEST--
Allow abstract function override
--FILE--
<?php

abstract class A           { abstract function bar($x, $y = 0); }
abstract class B extends A { abstract function bar($x); }

echo "DONE";
?>
--EXPECTF--
Fatal error: Declaration of B::bar($x) must be compatible with A::bar($x, $y = 0) in %s
