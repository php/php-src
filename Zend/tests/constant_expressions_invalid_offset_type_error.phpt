--TEST--
Can't use arrays as key for constant array
--FILE--
<?php

const C1 = 1; // force dynamic evaluation
const C2 = [C1, [] => 1];

?>
--EXPECTF--
Fatal error: Illegal offset type in %s on line %d
