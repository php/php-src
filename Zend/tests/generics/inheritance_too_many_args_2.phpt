--TEST--
Passing too many generic args to parent (2)
--FILE--
<?php

abstract class P<T, T2 = string> {
}

class C extends P<int, int, int> {
}

?>
--EXPECTF--
Fatal error: Class P expects at most 2 generic arguments, but 3 provided in %s on line %d
