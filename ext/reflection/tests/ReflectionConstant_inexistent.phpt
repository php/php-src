--TEST--
ReflectionConstant on inexistent constant
--FILE--
<?php

try {
    new \ReflectionConstant('C');
} catch (Exception $e) {
    echo get_class($e) . ': ' . $e->getMessage() . "\n";
}

?>
--EXPECT--
ReflectionException: Constant "C" does not exist
