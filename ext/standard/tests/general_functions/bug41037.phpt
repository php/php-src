--TEST--
Bug #41037 (unregister_tick_function() inside the tick function crash PHP)
--FILE--
<?php

function a() {
    echo "hello";
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
--EXPECT--
helloRegistered tick function cannot be unregistered while it is being executed
Done
helloRegistered tick function cannot be unregistered while it is being executed
