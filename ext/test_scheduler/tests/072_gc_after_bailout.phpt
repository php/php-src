--TEST--
test_scheduler: a GC run cut off by a bailout does not leave its coroutine behind
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php
declare(ticks=1);

class Node {
    public $self;

    public function __construct(public string $name) {}

    public function __destruct()
    {
        echo "destructor {$this->name}\n";
    }
}

function make_cycle(string $name): void
{
    $node = new Node($name);
    $node->self = $node;
}

// After the bailout this run must collect, not await the coroutine the fatal
// error cut off. The fatal error marked the tick's node destructed, so it is
// freed without a destructor call.
register_shutdown_function(function () {
    echo "shutdown function\n";
    make_cycle('shutdown');
    var_dump(gc_collect_cycles());
});

// Switching is blocked in a tick function: the GC coroutine is only queued,
// and the fatal error below comes before it runs.
$ticked = false;
register_tick_function(function () use (&$ticked) {
    if ($ticked) {
        return;
    }

    $ticked = true;
    make_cycle('tick');
    var_dump(gc_collect_cycles());
});

echo "fatal next\n";
trigger_error('fatal', E_USER_ERROR);
?>
--EXPECTF--
int(0)
fatal next

Deprecated: Passing E_USER_ERROR to trigger_error() is deprecated since 8.4, throw an exception or call exit with a string message instead in %s on line %d

Fatal error: fatal in %s on line %d
shutdown function
int(1)
destructor shutdown
