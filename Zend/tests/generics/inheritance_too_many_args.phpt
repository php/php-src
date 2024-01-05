--TEST--
Passing too many generic args to parent
--FILE--
<?php

abstract class P<T, T2> {
}

class C extends P<int, int, int> {
}

?>
--EXPECTF--
Fatal error: Class P expects exactly 2 generic arguments, but 3 provided in %s on line %d
