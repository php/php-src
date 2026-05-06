--TEST--
Scope-fn generator created in parent, captured by a Fiber: parent-exit cleanup destructs the generator
--FILE--
<?php
$fiber = null;
$gen = null;
function outer() {
    global $fiber, $gen;
    $x = 100;
    $fn = fn() {
        yield $x;
        $x++;
        yield $x;
    };
    $gen = $fn();
    var_dump($gen->current()); // int(100)

    $fiber = new Fiber(function () {
        global $gen;
        Fiber::suspend("paused");
        /* When user resumes us, the generator has been force-destructed
         * (parent exited). It reports as already finished. */
        echo "valid: ", var_export($gen->valid(), true), "\n";
        try {
            $gen->next();
        } catch (Throwable $e) {
            echo "next: ", $e->getMessage(), "\n";
        }
    });
    var_dump($fiber->start()); // string(6) "paused"
    /* outer returns: $gen and $fiber both still alive. The closure is
     * referenced by the generator; the generator's parent-exit cleanup
     * force-destructs it. The fiber doesn't reference the closure
     * directly (only via $gen) so no fiber unwind happens. */
}

try {
    outer();
} catch (Error $e) {
    echo "caught: ", $e->getMessage(), "\n";
}

/* Pollute the vm_stack region that held outer's frame (and scope_ex): if
 * the generator or fiber kept a stale pointer into that region, the next
 * access would land in overwritten memory. */
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

/* Generator is dead. Fiber is alive, suspended. Resume it. */
echo "post-outer: gen valid=", var_export($gen->valid(), true), "\n";
$fiber->resume();

$fiber = null;
$gen = null;
echo "done\n";
?>
--EXPECT--
int(100)
string(6) "paused"
caught: Scope function closure must not outlive the declaring scope
post-outer: gen valid=false
valid: false
done
