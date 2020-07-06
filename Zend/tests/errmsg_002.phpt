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
Fatal error: Abstract method test::foo() must have public or protected visibility in %s on line %d
