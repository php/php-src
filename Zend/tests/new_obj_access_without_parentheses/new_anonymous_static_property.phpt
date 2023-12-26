--TEST--
Immediate static property access on new anonymous class object
--FILE--
<?php

echo new class {
    public static $staticProperty = 'staticProperty';
}::$staticProperty;

?>
--EXPECT--
staticProperty
