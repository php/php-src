--TEST--
Value classes cannot have dynamic properties
--FILE--
<?php
value class EmptyValue {}
$value = new EmptyValue;
try {
    $value->extra = 1;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    unserialize('O:10:"EmptyValue":1:{s:5:"extra";i:1;}');
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Cannot create dynamic property EmptyValue::$extra
Cannot create dynamic property EmptyValue::$extra
