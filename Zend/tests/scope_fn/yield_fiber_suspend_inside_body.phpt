--TEST--
Fiber::suspend called from within a scope-fn generator's body, then parent exits
--FILE--
<?php
$fiber = null;
function outer() {
    global $fiber;
    $fn = fn() {
        yield 1;
        Fiber::suspend("inside-gen");
        yield 2;
    };
    $g = $fn();
    var_dump($g->current()); // 1

    $fiber = new Fiber(function () use ($g) {
        try {
            $g->next();
            echo "after next (unreachable)\n";
        } catch (Throwable $e) {
            /* The throw materializes here, inside the fiber, at the
             * suspension resumption point ($g->next() returns into the
             * fiber with the Error in flight). The stacktrace shows the
             * Error was created inside the generator's body. */
            echo $e, "\n";
        }
    });
    /* Wrapping start() in try/catch demonstrates that start() does NOT
     * throw — the deferred Error is injected into the fiber on its next
     * resume, not surfaced through start(). */
    try {
        var_dump($fiber->start()); // string(10) "inside-gen"
    } catch (Throwable $e) {
        echo "start caught (unreachable): ", $e->getMessage(), "\n";
    }
}

try {
    outer();
} catch (Error $e) {
    echo "outer: ", $e->getMessage(), "\n";
}

/* Pollute the vm_stack region that held outer's frame (and scope_ex): if
 * the generator's force-destruct or the fiber's saved state retained a
 * stale pointer into outer's frame, the next access lands in overwritten
 * memory. */
function churn(int $depth): int {
    $a=1; $b=2; $c=3; $d=4; $e=5; $f=6; $g=7; $h=8;
    $i=9; $j=10; $k=11; $l=12; $m=13; $n=14; $o=15; $p=16;
    $q=17; $r=18; $s=19; $t=20; $u=21; $v=22; $w=23; $x=24;
    if ($depth > 0) return churn($depth - 1) + $a + $x;
    return $a + $b + $c + $d + $e + $f + $g + $h
         + $i + $j + $k + $l + $m + $n + $o + $p
         + $q + $r + $s + $t + $u + $v + $w + $x;
}
for ($iter = 0; $iter < 100; $iter++) churn(50);

/* Resume the fiber: the deferred Error is injected at the fiber's
 * suspension resumption point, the fiber body's catch above sees it and
 * prints the trace, then the fiber finishes naturally (NULL). */
var_dump($fiber->resume());

$fiber = null;
echo "done\n";
?>
--EXPECTF--
int(1)
string(10) "inside-gen"
outer: Scope function closure must not outlive the declaring scope
Error: Scope function closure must not outlive the declaring scope in %syield_fiber_suspend_inside_body.php:%d
Stack trace:
#0 [internal function]: {closure:%s}()
#1 %syield_fiber_suspend_inside_body.php(%d): Generator->next()
#2 [internal function]: {closure:%s}()
#3 %syield_fiber_suspend_inside_body.php(%d): unknown()
#4 {main}
NULL
done
