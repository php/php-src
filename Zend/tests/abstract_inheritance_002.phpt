--TEST--
Allow abstract function override
--FILE--
<?php

abstract class A           { abstract function bar($x); }
abstract class B extends A { abstract function bar($x, $y = 0); }

echo "DONE";
?>
--EXPECT--
DONE
