--TEST--
test_scheduler: an automatic GC run refused where switching is blocked throws nowhere
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php
declare(ticks=1);

$objects = [];
for ($i = 0; $i < 12000; $i++) {
    $objects[] = new stdClass;
}

$ticked = false;

register_tick_function(function () use (&$ticked, $objects) {
    if ($ticked) {
        return;
    }

    $ticked = true;

    // The GC coroutine is the next coroutine queued.
    ini_set('test_scheduler.fail_enqueue', 1);

    // Live objects: each one becomes a possible root, and the buffer fills.
    foreach ($objects as $object) {
        $copy = $object;
        unset($copy);
    }

    echo "tick done\n";
});

echo "after tick\n";
var_dump(gc_status()['runs']);
?>
--EXPECT--
tick done
after tick
int(0)
