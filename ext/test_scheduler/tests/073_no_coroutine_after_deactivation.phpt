--TEST--
test_scheduler: no current coroutine once the request deactivates async
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php
// The final flush of output buffers runs after async is deactivated, when
// the request's coroutines are being torn down.
ob_start(function (string $buffer, int $phase): string {
    $current = TestScheduler\current();
    $name = $current === null ? "null" : get_class($current);

    return $buffer . "current in phase $phase: $name\n";
});

echo "main\n";
ob_flush();
?>
--EXPECT--
main
current in phase 5: TestScheduler\Coroutine
current in phase 8: null
