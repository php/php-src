--TEST--
Test that there is no arginfo/zpp mismatch
--SKIPIF--
<?php
if (getenv('SKIP_ASAN')) die("skip Intermittently crashes lsan");
if (getenv('SKIP_MSAN')) die("skip msan misses interceptors for some functions");
?>
--FILE--
<?php

require __DIR__ . "/arginfo_zpp_mismatch.inc";

function testWeakSysvIpcFunction($function): bool {
    if (!in_array($function, ['msg_queue_exists', 'msg_get_queue', 'sem_get', 'shm_attach'], true)) {
        return false;
    }

    for ($argumentCount = 0; $argumentCount <= 8; $argumentCount++) {
        $arguments = array_fill(0, $argumentCount, null);
        if ($function === 'msg_queue_exists' && $argumentCount >= 1) {
            $arguments[0] = 1;
        }
        if (($function === 'sem_get' || $function === 'shm_attach') && $argumentCount >= 3) {
            $arguments[2] = 0600;
        }

        try {
            $result = @$function(...$arguments);
            if ($result instanceof SysvMessageQueue) {
                msg_remove_queue($result);
            } elseif ($result instanceof SysvSemaphore) {
                sem_remove($result);
            } elseif ($result instanceof SysvSharedMemory) {
                shm_remove($result);
            }
        } catch (Throwable) {
        }
    }

    return true;
}

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
    if (testWeakSysvIpcFunction($function)) {
        ob_end_clean();
        return;
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
