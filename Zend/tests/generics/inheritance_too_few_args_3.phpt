--TEST--
Passing too few generic args to parent (3)
--FILE--
<?php

abstract class P<T> {
}

class C extends P {
}

?>
--EXPECTF--
Fatal error: Class P expects exactly 1 generic argument, but 0 provided in %s on line %d
