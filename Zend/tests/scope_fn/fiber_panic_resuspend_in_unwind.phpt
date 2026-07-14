--TEST--
Forced unwind through a fiber's scope-fn body runs only finally blocks; sentinel is invisible to catch
--FILE--
<?php
$fiber = null;
function outer() {
    global $fiber;
    $fn = fn() {
        try {
            try {
                Fiber::suspend("paused");
                echo "after suspend (unreachable)\n";
            } catch (Throwable $e) {
                echo "catch fired (unreachable): ", $e->getMessage(), "\n";
            } finally {
                echo "finally fired\n";
            }
            echo "after try (unreachable)\n";
        } catch (Throwable $e) {
            echo "outer catch fired (unreachable): ", $e->getMessage(), "\n";
        }
    };
    $fiber = new Fiber($fn);
    var_dump($fiber->start()); // string(6) "paused"
}
try { outer(); } catch (Error $e) { echo "outer escape: ", $e->getMessage(), "\n"; }
$fiber = null;
echo "done\n";
?>
--EXPECT--
string(6) "paused"
finally fired
outer escape: Scope function closure must not outlive the declaring scope
done
