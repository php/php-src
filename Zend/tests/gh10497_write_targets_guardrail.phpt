--TEST--
GH-10497: Destructuring into a constant without property assignment
--FILE--
<?php
const ARR = [1, 2, 3];
[ARR[0]] = [9];
?>
--EXPECTF--
Fatal error: Assignments can only happen to writable values in %s on line %d
