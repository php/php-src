--TEST--
Test the case where the initializing property assignment fails
--FILE--
<?php
class Test {
    public int $prop { get; }
}
$b = new Test;
try {
    $b->prop = "foo";
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    $b->prop;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Cannot assign string to property Test::$prop of type int
Property Test::$prop must not be accessed before initialization
