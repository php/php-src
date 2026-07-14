--TEST--
Forced unwind transits through a non-scope-fn generator's resume to reach the surrounding scope_fn boundary
--FILE--
<?php
/* A regular (non-scope-fn) generator suspends the surrounding fiber via
 * Fiber::suspend(). Its resume call sits on the fiber's saved chain INSIDE
 * a scope_fn body. When the scope_fn's declaring scope exits, the fiber with
 * the unwind exception; it must propagate THROUGH the generator's resume
 * so the scope_ex frame above can reach its leave_helper boundary. */
function regular_gen() {
    try {
        Fiber::suspend("from-regular-gen");
    } finally {
        echo "regular_gen finally\n";
    }
    yield 1; /* unreachable after force-unwind */
}

$fiber = null;
function outer() {
    global $fiber;
    $sfn = fn() {
        $g = regular_gen();
        try {
            $g->current();
        } finally {
            echo "scope_fn finally\n";
        }
        echo "unreachable\n";
    };
    $fiber = new Fiber($sfn);
    var_dump($fiber->start());
}

try {
    outer();
} catch (Error $e) {
    echo "outer caught: ", $e->getMessage(), "\n";
}

try {
    var_dump($fiber->resume());
} catch (Throwable $e) {
    echo "resume caught: ", $e::class, ": ", $e->getMessage(), "\n";
}
echo "done\n";
?>
--EXPECT--
string(16) "from-regular-gen"
regular_gen finally
scope_fn finally
outer caught: Scope function closure must not outlive the declaring scope
resume caught: Error: Scope function closure must not outlive the declaring scope
done
