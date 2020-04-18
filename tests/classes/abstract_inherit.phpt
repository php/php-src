--TEST--
ZE2 A class that inherits an abstract method is abstract
--FILE--
<?php

abstract class pass {
    abstract function show();
}

abstract class fail extends pass {
}

$t = new fail();
$t = new pass();

echo "Done\n"; // Shouldn't be displayed
?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot instantiate abstract class fail in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d 
