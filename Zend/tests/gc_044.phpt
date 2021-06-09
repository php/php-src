--TEST--
GC with __destruct, single gc_collect_cycles() call must be enough
--FILE--
<?php

class X {
    public $r;
    public $r2;

    public function __destruct() {
        $this->z = $this->r;
        $thisR2 = $this->r2;
        $this->r2 = $this->r2->r2;
        $this->r2->r2 = $thisR2;
    }
}

function getCircle(): X {
    $a = new X();
    $b = new X();
    $c = new X();
    $a->r = $a->r2 = $c;
    $b->r = $b->r2 = $a;
    $c->r = $c->r2 = $b;
    
    return $a;
}

function getNestedCircles(int $depth, X $first = null): X {
    $u = getCircle();
    
    if ($first === null) {
        $first = $u;
    }

    $depth--;
    if ($depth > 0) {
        $u->r2 = getNestedCircles($depth, $first);
    } else {
        $u->r2 = $first;
    }
    $u->r->r2 = $u;
    
    return $u;
}

$n = 20 * 1000;

// warm class buffers
getNestedCircles($n);
for ($i = 0; $i < $n * 2; $i++) {
    gc_collect_cycles();
}

$zeroMemory = memory_get_usage();
$u = getNestedCircles($n);
$u = null;

for ($i = 0; $i < 5; $i++) {
    gc_collect_cycles();
    var_dump(memory_get_usage() - $zeroMemory);
}
?>
--EXPECT--
int(0)
int(0)
int(0)
int(0)
int(0)
