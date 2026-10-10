--TEST--
test_scheduler: a generator advances across coroutine switches
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php
use function TestScheduler\{spawn, suspend, resume};

function gen(): Generator {
    for ($i = 0; $i < 3; $i++) {
        yield "key-" . $i => "value-" . $i;
    }
}

$consumer = null;

$driver = spawn(function () use (&$consumer) {
    suspend();
    while (!$consumer->isFinished()) {
        resume($consumer);
        suspend();
    }
    echo "driver done\n";
});

$consumer = spawn(function () use ($driver) {
    foreach (gen() as $key => $value) {
        echo "$key => $value\n";
        resume($driver);
        suspend();
    }
    echo "consumer done\n";
    resume($driver);
});

echo "main done\n";
?>
--EXPECT--
main done
key-0 => value-0
key-1 => value-1
key-2 => value-2
consumer done
driver done
