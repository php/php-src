--TEST--
GH-16477: Segmentation fault when calling __debugInfo() after failed SplFileObject::__constructor
--FILE--
<?php

$obj = new SplFileObject(__FILE__);

try {
    $obj->__construct();
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
$obj->__debugInfo();

?>
DONE
--EXPECT--
ArgumentCountError: SplFileObject::__construct() expects at least 1 argument, 0 given
DONE
