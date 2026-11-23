--TEST--
Zend: Test zend_forbid_dynamic_call() for static methods in pipe and array_map()
--EXTENSIONS--
zend_test
--FILE--
<?php

use ZendTestForbidDynamicCall as FDC;

try {
    1 |> ZendTestForbidDynamicCall::callStatic(?);
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    1 |> \ZendTestForbidDynamicCall::callStatic(?);
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    1 |> FDC::callStatic(?);
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    1 |> ('ZendTestForbidDynamicCall::callStatic')(?);
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    1 |> ('\ZendTestForbidDynamicCall::callStatic')(?);
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    array_map(ZendTestForbidDynamicCall::callStatic(?), []);
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    array_map(FDC::callStatic(?), []);
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    array_map(('ZendTestForbidDynamicCall::callStatic')(?), []);
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Error: Cannot call callStatic() dynamically
Error: Cannot call callStatic() dynamically
Error: Cannot call callStatic() dynamically
Error: Cannot call callStatic() dynamically
Error: Cannot call callStatic() dynamically
Error: Cannot call callStatic() dynamically
Error: Cannot call callStatic() dynamically
Error: Cannot call callStatic() dynamically
