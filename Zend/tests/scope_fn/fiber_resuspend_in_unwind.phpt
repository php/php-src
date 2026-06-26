--TEST--
Fiber::suspend called from a finally block during scope-fn force-unwind throws FiberError
--FILE--
<?php
$fiber = null;
function outer() {
    global $fiber;
    $fn = fn() {
        try {
            Fiber::suspend("paused");
        } finally {
            echo "finally entered\n";
            try {
                Fiber::suspend("from-finally");
                echo "after resuspend (unreachable)\n";
            } catch (FiberError $e) {
                echo "caught: ", $e->getMessage(), "\n";
            }
            echo "finally exiting\n";
        }
    };
    $fiber = new Fiber($fn);
    var_dump($fiber->start()); // "paused"
}

try {
    outer();
} catch (Error $e) {
    echo "outer: ", $e->getMessage(), "\n";
}
$fiber = null;
echo "done\n";
?>
--EXPECT--
string(6) "paused"
finally entered
caught: Cannot suspend in a force-closed fiber
finally exiting
outer: Scope function closure must not outlive the declaring scope
done
