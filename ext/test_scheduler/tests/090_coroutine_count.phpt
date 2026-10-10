--TEST--
test_scheduler: ZEND_ASYNC_GET_COROUTINE_COUNT() counts the request's unfinished coroutines, the main one included
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php
use function TestScheduler\{spawn, await, coroutineCount};

var_dump(coroutineCount());

$first = spawn(function () {
    echo "first: ", coroutineCount(), "\n";
});
// Runs after the first has finished.
$second = spawn(fn() => coroutineCount());

var_dump(coroutineCount());

await($first);
var_dump(await($second));
var_dump(coroutineCount());
?>
--EXPECT--
int(1)
int(3)
first: 3
int(2)
int(1)
