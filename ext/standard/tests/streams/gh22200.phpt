--TEST--
GH-22200 (Memory leaks in the stream errors code on reentrant error handler)
--FILE--
<?php

// When the warning emitted for the failing outer stream operation is reported,
// php_stream_error_operation_end() has already popped the operation off the
// stack. The user error handler then runs another stream operation, which
// reuses the same operation pool slot and orphans the in-flight error entry,
// leaking it.
set_error_handler(function ($errno, $errstr) {
    @file_get_contents(__DIR__ . '/gh22200-does-not-exist-inner');
    return true;
});

file_get_contents(__DIR__ . '/gh22200-does-not-exist-outer');

echo "done\n";

?>
--EXPECT--
done
