--TEST--
GH-16477-2: Memory leak when calling SplTempFileObject::__constructor() twice
--FILE--
<?php

$obj = new SplTempFileObject();

try {
    $obj->__construct();
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
$obj->__debugInfo();

?>
DONE
--EXPECT--
Error: Cannot call constructor twice
DONE
