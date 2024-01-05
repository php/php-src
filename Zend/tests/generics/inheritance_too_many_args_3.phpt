--TEST--
Passing too many generic args to parent (3)
--FILE--
<?php

abstract class P {
}

class C extends P<int, int, int> {
}

?>
--EXPECTF--
Fatal error: Class P expects exactly 0 generic arguments, but 3 provided in %s on line %d
