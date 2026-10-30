--TEST--
abstract final methods errmsg
--FILE--
<?php

class test {
    final abstract function foo();
}

echo "Done\n";
?>
--EXPECTF--
Fatal error: Cannot use the final modifier on an abstract method in %s on line %d
