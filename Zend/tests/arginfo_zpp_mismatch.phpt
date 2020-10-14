--TEST--
Test that there is no arginfo/zpp mismatch
--FILE--
<?php

function test($function) {
    if (false
        /* expect input / hang */
     || $function === 'readline'
     || $function === 'readline_read_history'
     || $function === 'readline_write_history'
        /* intentionally violate invariants */
     || $function === 'zend_create_unterminated_string'
     || $function === 'zend_test_array_return'
     || $function === 'zend_leak_bytes'
        /* mess with output */
     || (is_string($function) && str_starts_with($function, 'ob_'))
     || $function === 'output_add_rewrite_var'
     || $function === 'error_log'
        /* may spend a lot of time waiting for connection timeouts */
     || (is_string($function) && str_contains($function, 'connect'))
     || (is_string($function) && str_starts_with($function, 'snmp'))
     || (is_array($function) && get_class($function[0]) === mysqli::class
         && in_array($function[1], ['__construct', 'connect', 'real_connect']))
        /* misc */
     || $function === 'mail'
     || $function === 'mb_send_mail'
     || $function === 'pcntl_fork'
     || $function === 'posix_kill'
     || $function === 'posix_setrlimit'
     || $function === 'sapi_windows_generate_ctrl_event'
     || $function === 'imagegrabscreen'
    ) {
        return;
    }
    if ($function[0] instanceof SoapServer) {
        /* TODO: Uses fatal errors */
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
