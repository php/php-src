--TEST--
Bug #41037 (unregister_tick_function() inside the tick function crash PHP)
--FILE--
<?php

function a() {
    echo "hello\n";
    try {
        unregister_tick_function('a');
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }
}

declare (ticks=1) {
    register_tick_function('a');

    echo "Done\n";
}
?>
--EXPECTF--
Deprecated: declare(ticks) is deprecated in %s on line %d

Deprecated: Function register_tick_function() is deprecated in %s on line %d
hello

Deprecated: Function unregister_tick_function() is deprecated in %s on line %d
Registered tick function cannot be unregistered while it is being executed
Done
hello

Deprecated: Function unregister_tick_function() is deprecated in %s on line %d
Registered tick function cannot be unregistered while it is being executed
