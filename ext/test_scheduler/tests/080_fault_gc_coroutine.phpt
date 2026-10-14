--TEST--
test_scheduler: gc_collect_cycles() when its coroutine or its destructor iterator cannot be created or queued
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php
class Node {
    public $self;

    public function __destruct() {
        echo "destructor\n";
    }
}

function make_cycle() {
    $node = new Node();
    $node->self = $node;
}

/* The GC coroutine is the first coroutine the call creates, the destructor
 * iterator the second. */
$faults = [['fail_new_coroutine', 1], ['fail_enqueue', 1], ['fail_new_coroutine', 2], ['fail_enqueue', 2]];
foreach ($faults as [$setting, $nth]) {
    echo "$setting=$nth\n";
    make_cycle();
    ini_set("test_scheduler.$setting", $nth);

    try {
        var_dump(gc_collect_cycles());
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }

    ini_set("test_scheduler.$setting", 0);
    var_dump(gc_collect_cycles());
}
?>
--EXPECT--
fail_new_coroutine=1
int(0)
destructor
int(1)
fail_enqueue=1
Cannot enqueue the coroutine: test_scheduler.fail_enqueue
destructor
int(1)
fail_new_coroutine=2
destructor
int(0)
int(1)
fail_enqueue=2
destructor
int(0)
int(1)
