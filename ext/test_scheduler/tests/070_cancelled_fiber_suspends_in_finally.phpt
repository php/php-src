--TEST--
test_scheduler: a fiber cancelled by the scheduler may suspend in finally; the next cancel unwinds it
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php
// Left suspended when main ends: the drain cancels it, finally parks it
// again, and the drain's next cancel unwinds it from there.
$fiber = new Fiber(function () {
    try {
        Fiber::suspend();
    } finally {
        echo "first finally\n";

        try {
            Fiber::suspend();
            echo "unreachable\n";
        } finally {
            echo "second finally\n";
        }
    }
});

$fiber->start();

echo "main done\n";
?>
--EXPECT--
main done
first finally
second finally
