--TEST--
Scope-fn generator that outlives its parent: force-destructed at parent exit
--FILE--
<?php
$gen = null;
function outer() {
    global $gen;
    $x = 1;
    $fn = fn() {
        yield $x;
        yield $x + 1;
        yield $x + 2;
    };
    $gen = $fn();
    var_dump($gen->current()); // int(1)
    /* outer returns with $gen still suspended after first yield. Parent-exit
     * cleanup must force-destruct the generator before freeing parent's
     * frame, then throw the escape Error. */
}
try {
    outer();
    echo "no error?\n";
} catch (Error $e) {
    echo "caught: ", $e->getMessage(), "\n";
}

/* Force the parent's vm_stack region (where scope_ex lived) to be
 * reused by subsequent function calls. If the generator's saved state
 * wasn't properly torn down, this would clobber the scope_ex memory
 * the generator still references. */
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

/* Generator is dead. Subsequent ops report "already finished". */
var_dump($gen->valid());
try {
    $gen->next();
    var_dump($gen->current());
} catch (Throwable $e) {
    echo "next: ", $e->getMessage(), "\n";
}
$gen = null;
echo "done\n";
?>
--EXPECT--
int(1)
caught: Scope function closure must not outlive the declaring scope
bool(false)
NULL
done
