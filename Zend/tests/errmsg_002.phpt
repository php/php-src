--TEST--
errmsg: function cannot be declared private
--FILE--
<?php

abstract class test {
    abstract private function foo() {
    }
}

echo "Done\n";
?>
--EXPECTF--
Fatal error: Abstract function test::foo() cannot be declared private in %s on line %d
