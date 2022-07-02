--TEST--
ZE2 An abstract class cannot be instantiated
--FILE--
<?php

abstract class base {
    function show() {
        echo "base\n";
    }
}

class derived extends base {
}

$t = new derived();
$t->show();

$t = new base();
$t->show();

echo "Done\n"; // shouldn't be displayed
?>
--EXPECTF--
base

Fatal error: Uncaught Error: Cannot instantiate abstract class base in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
