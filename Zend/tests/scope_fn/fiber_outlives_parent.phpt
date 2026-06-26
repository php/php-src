--TEST--
Cross-stack scope fn used as fiber body: parent exit drives forced unwind
--FILE--
<?php
$fiber = null;
function outer() {
    global $fiber;
    $x = 42;
    $fn = fn() {
        Fiber::suspend("paused");
    };
    $fiber = new Fiber($fn);
    var_dump($fiber->start()); // string(6) "paused"
}

try {
    outer();
    echo "no error?\n";
} catch (Error $e) {
    echo "caught at outer return: ", $e->getMessage(), "\n";
}

/* Pollute the vm_stack region outer used (where scope_ex lived) so any
 * stale pointer the fiber retained would land in overwritten memory. */
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

/* Fiber was force-unwound through scope_ex at outer's exit; the visible
 * escape Error is stashed on the fiber and surfaces here. */
try {
    $fiber->resume();
    echo "no resume error?\n";
} catch (Throwable $e) {
    echo "caught at resume: ", $e->getMessage(), "\n";
}

/* A subsequent resume gets the regular "not suspended" error (the deferred
 * throw is one-shot). */
try {
    $fiber->resume();
} catch (Throwable $e) {
    echo "caught at second resume: ", $e->getMessage(), "\n";
}

$fiber = null;
echo "done\n";
?>
--EXPECT--
string(6) "paused"
caught at outer return: Scope function closure must not outlive the declaring scope
caught at resume: Scope function closure must not outlive the declaring scope
caught at second resume: Cannot resume a fiber that is not suspended
done
