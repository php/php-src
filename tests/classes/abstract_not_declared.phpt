--TEST--
ZE2 An abstract class must be declared abstract
--FILE--
<?php

class fail {
    abstract function show();
}

echo "Done\n"; // shouldn't be displayed
?>
--EXPECTF--
Fatal error: Class fail declares abstract method show() and must therefore be declared abstract in %s on line %d
