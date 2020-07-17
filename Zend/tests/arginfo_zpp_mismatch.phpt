--TEST--
Test that there is no arginfo/zpp mismatch
--FILE--
<?php

function test($function) {
    try {
        @$function(null, null, null, null, null, null, null, null);
    } catch (Throwable) {
    }
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
--EXPECT--
NULL
NULL
NULL
NULL
NULL
NULL
NULL
NULL
NULL
NULL
NULL
NULL
NULL
NULL
NULL
NULL
