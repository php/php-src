--TEST--
GC collects a cycle running through spawn()'s named arguments
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php

$obj = new stdClass();
$co = TestScheduler\spawn(function (mixed ...$args) { return 1; }, tag: $obj);
$obj->self = $co;

TestScheduler\cancel($co);
TestScheduler\await(TestScheduler\spawn(fn () => 1));   // let the loop tick

$weak = WeakReference::create($co);
unset($co, $obj);

var_dump(gc_collect_cycles() > 0);
var_dump($weak->get() === null);

echo "==DONE==\n";
?>
--EXPECT--
bool(true)
bool(true)
==DONE==
