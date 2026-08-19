--TEST--
Bug #41037 (unregister_tick_function() inside the tick function crash PHP)
--FILE--
<?php

function a() {
    echo "hello\n";
    try {
        unregister_tick_function('a');
    } catch (Throwable $exception) {
        echo $exception::class, ': ', $exception->getMessage(), "\n";
    }
}

declare (ticks=1) {
    register_tick_function('a');

    echo "Done\n";
}
?>
--EXPECT--
hello
Error: Registered tick function cannot be unregistered while it is being executed
Done
hello
Error: Registered tick function cannot be unregistered while it is being executed
