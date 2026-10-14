--TEST--
test_scheduler: GC where switching is blocked starts its coroutine and returns at once
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php
declare(ticks=1);

class Node {
    public $self;

    public function __destruct()
    {
        echo "destructor\n";
    }
}

function make_cycle(): void
{
    $node = new Node;
    $node->self = $node;
}

// Live objects whose refcount drops to non-zero: each one becomes a possible
// root that is not garbage, so an automatic run collects nothing.
function fill_roots(array $objects): void
{
    foreach ($objects as $object) {
        $copy = $object;
        unset($copy);
    }
}

$objects = [];
for ($i = 0; $i < 12000; $i++) {
    $objects[] = new stdClass;
}

$threshold = gc_status()['threshold'];
$ticked = false;

register_tick_function(function () use (&$ticked, $objects) {
    if ($ticked) {
        return;
    }

    $ticked = true;
    make_cycle();
    var_dump(gc_collect_cycles());
    fill_roots($objects);
    echo "tick done\n";
});

echo "after tick\n";
var_dump(gc_status()['threshold'] === $threshold);
// The deferred run collects the cycle when main ends and the queue drains.
?>
--EXPECT--
int(0)
tick done
after tick
bool(true)
destructor
