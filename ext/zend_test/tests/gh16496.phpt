--TEST--
GH-16496: _ZendTestFiber failed assertion when started twice
--EXTENSIONS--
zend_test
--FILE--
<?php

$test = new _ZendTestFiber(function () {});
$test->start();

try {
    $test->start();
} catch(FiberError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Cannot start a fiber that has already been started
