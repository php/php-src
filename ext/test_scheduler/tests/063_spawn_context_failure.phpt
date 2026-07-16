--TEST--
A spawn() failing to create its fiber context leaves no phantom coroutine behind
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php

ini_set('fiber.stack_size', '1');

try {
    TestScheduler\spawn(fn () => 1);
    echo "spawn unexpectedly succeeded\n";
} catch (Throwable $e) {
    echo get_class($e), ": ", $e->getMessage(), "\n";
}

ini_set('fiber.stack_size', '1048576');

var_dump(TestScheduler\await(TestScheduler\spawn(fn () => "ok")));

echo "==DONE==\n";
?>
--EXPECTF--
Exception: Fiber stack size is too small, it needs to be at least %d bytes
string(2) "ok"
==DONE==
