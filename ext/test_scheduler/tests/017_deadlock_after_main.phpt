--TEST--
test_scheduler: a mutually-awaiting pair left behind by main is closed gracefully
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php
use function TestScheduler\{spawn, await, suspend};

$c1 = null;
$c2 = null;

$c1 = spawn(function () use (&$c2) {
    try {
        echo "c1 running\n";
        suspend();
        await($c2);
        echo "c1 unreachable\n";
    } finally {
        echo "c1 finally\n";
    }
});

$c2 = spawn(function () use (&$c1) {
    try {
        echo "c2 running\n";
        suspend();
        await($c1);
        echo "c2 unreachable\n";
    } finally {
        echo "c2 finally\n";
    }
});

echo "main done\n";
?>
--EXPECT--
main done
c1 running
c2 running
c1 finally
c2 finally
