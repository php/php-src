--TEST--
await inside a plain Fiber drives the scheduler
--FILE--
<?php
$task = Async\async(function () {
    await Async\delay(0.01);
    return "task";
});
$fiber = new Fiber(function () use ($task) {
    return await $task;
});
$fiber->start();
var_dump($fiber->getReturn());
?>
--EXPECT--
string(4) "task"
