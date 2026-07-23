--TEST--
test_scheduler: a frame-held object of a parked coroutine survives GC, collects after use
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php
use function TestScheduler\{spawn, suspend, resume, current};

$weak = null;

class Cyc {
    public $self;
    public function __construct() {
        global $weak;
        $this->self = $this; // a GC candidate
        $weak = WeakReference::create($this);
    }
}

function op(Cyc $c, $x) { return $c; }

$main = current();

$co = spawn(function () {
    // While parked, the Cyc lives only in the in-flight argument slot.
    $r = op(new Cyc(), suspend());
    echo "resumed, self ", ($r->self === $r ? "intact" : "broken"), "\n";
});

spawn(function () use ($co, $main) {
    gc_collect_cycles();
    global $weak;
    echo "after gc: ", ($weak->get() === null ? "collected" : "alive"), "\n";
    resume($co);
    resume($main);
});

suspend();
gc_collect_cycles();
echo "end: ", ($weak->get() === null ? "collected" : "alive"), "\n";
?>
--EXPECT--
after gc: alive
resumed, self intact
end: collected
