--TEST--
IO hooks: the sleep family is a Timer operation and fibers sleep concurrently
--FILE--
<?php

include __DIR__ . '/scheduler.inc';

$scheduler = new Scheduler();
Io\Hooks\set_hooks($scheduler);

$start = hrtime(true);
$scheduler->spawn(function () {
    usleep(60000);
    echo "usleep done\n";
});
$scheduler->spawn(function () {
    time_nanosleep(0, 20000000);
    echo "time_nanosleep done\n";
});
$scheduler->spawn(function () {
    var_dump(sleep(0));
    echo "sleep done\n";
});
$scheduler->loop();

$elapsed_ms = (hrtime(true) - $start) / 1e6;
var_dump($elapsed_ms >= 60, $elapsed_ms < 500);
var_dump(Io\Hooks\get_hooks() === $scheduler, Io\Hooks\is_active());
var_dump(Io\Hooks\set_hooks(null) === $scheduler, Io\Hooks\is_active());
?>
--EXPECT--
int(0)
sleep done
time_nanosleep done
usleep done
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
