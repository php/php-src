--TEST--
ZE2 An abstract method may not be called
--FILE--
<?php

abstract class fail {
    abstract function show();
}

class pass extends fail {
    function show() {
        echo "Call to function show()\n";
    }
    function error() {
        parent::show();
    }
}

$t = new pass();
$t->show();
$t->error();

echo "Done\n"; // shouldn't be displayed
?>
--EXPECTF--
Call to function show()

Fatal error: Uncaught Error: Cannot call abstract method fail::show() in %s:%d
Stack trace:
#0 %s(%d): pass->error()
#1 {main}
  thrown in %s on line %d
