--TEST--
Passing too few generic args to parent (2)
--FILE--
<?php

abstract class P<T, T2, T3 = string> {
}

class C extends P<int> {
}

?>
--EXPECTF--
Fatal error: Class P expects at least 2 generic arguments, but 1 provided in %s on line %d
