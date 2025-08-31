--TEST--
Test that there is no arginfo/zpp mismatch in strict mode
--SKIPIF--
<?php
if (getenv('SKIP_ASAN')) die("skip Intermittently crashes lsan");
if (getenv('SKIP_MSAN')) die("skip msan misses interceptors for some functions");
?>
--FILE--
<?php

declare(strict_types=1);

require __DIR__ . "/arginfo_zpp_mismatch.inc";

function test($function) {
    if (skipFunction($function)) {
        return;
    }

    ob_start();
    if (is_string($function)) {
        echo "Testing $function\n";
    } else {
        echo "Testing " . get_class($function[0]) . "::$function[1]\n";
    }
    try {
        @$function();
    } catch (Throwable) {
    }
    try {
        @$function(null);
    } catch (Throwable) {
    }
    try {
        @$function(null, null);
    } catch (Throwable) {
    }
    try {
        @$function(null, null, null);
    } catch (Throwable) {
    }
    try {
        @$function(null, null, null, null);
    } catch (Throwable) {
    }
    try {
        @$function(null, null, null, null, null);
    } catch (Throwable) {
    }
    try {
        @$function(null, null, null, null, null, null);
    } catch (Throwable) {
    }
    try {
        @$function(null, null, null, null, null, null, null);
    } catch (Throwable) {
    }
    try {
        @$function(null, null, null, null, null, null, null, null);
    } catch (Throwable) {
    }
    ob_end_clean();
}

foreach (get_defined_functions()["internal"] as $function) {
    test($function);
}

foreach (get_declared_classes() as $class) {
    try {
        $rc = new ReflectionClass($class);
        $obj = $rc->newInstanceWithoutConstructor();
    } catch (Throwable) {
        continue;
    }

    foreach (get_class_methods($class) as $method) {
        test([$obj, $method]);
    }
}

// var_dump() and debug_zval_dump() print all arguments
?>
===DONE===
--EXPECT--
===DONE===
