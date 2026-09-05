--TEST--
test_scheduler: cancelling a coroutine parked in a GC run exits cleanly, no leaks
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php
use function TestScheduler\{spawn, suspend, resume, cancel, current};

class Cyc {
    public $self;
    public function __destruct() {
        echo "dtor suspends\n";
        suspend();
        echo "dtor resumed\n";
    }
}

$main = current();

$gcer = spawn(function () {
    $a = new Cyc;
    $a->self = $a;
    unset($a);

    try {
        gc_collect_cycles();
        echo "gc returned\n";
    } catch (TestScheduler\CancellationError $e) {
        echo "gcer cancelled\n";
    }
});

spawn(function () use ($gcer, $main) {
    cancel($gcer);
    resume($main);
});

suspend();
echo "main done\n";
?>
--EXPECT--
gcer cancelled
main done
dtor suspends
