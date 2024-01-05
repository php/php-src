--TEST--
Passing too few generic args to parent
--FILE--
<?php

abstract class P<T, T2> {
}

class C extends P<int> {
}

?>
--EXPECTF--
Fatal error: Class P expects exactly 2 generic arguments, but 1 provided in %s on line %d
