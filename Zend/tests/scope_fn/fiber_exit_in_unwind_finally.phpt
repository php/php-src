--TEST--
exit() in a finally block of a fiber being force-unwound by scope-fn must not be swallowed
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
            exit("EXIT FROM FINALLY\n");
        }
    };
    $fiber = new Fiber($fn);
    var_dump($fiber->start());
}
try { outer(); } catch (Throwable $e) { echo "MUST NOT REACH (caught): ", $e->getMessage(), "\n"; }
$fiber = null;
echo "MUST NOT REACH (after)\n";
?>
--EXPECT--
string(6) "paused"
finally entered
EXIT FROM FINALLY
