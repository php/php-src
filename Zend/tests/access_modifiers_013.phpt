--TEST--
Allow abstract and final in the same class declaration
--FILE--
<?php

final abstract class C {
    private function priv() { }
}

echo "OK\n";

?>
--EXPECTF--
OK
