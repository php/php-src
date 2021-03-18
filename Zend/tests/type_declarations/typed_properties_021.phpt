--TEST--
Test typed properties delay type check on constant
--FILE--
<?php
try {
    class Foo {
        public int $bar = BAR::BAZ;
    }
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Class "BAR" not found
