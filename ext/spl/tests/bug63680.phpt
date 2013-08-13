--TEST--
Bug #63680 (Memleak in splfixedarray with cycle reference)
--FILE--
<?php
function dummy() {
    $a = new SplFixedArray(1);
    $b = new SplFixedArray(1);
    $a[0] = $b;
    $b[0] = $a;
}

dummy();
var_dump(gc_collect_cycles());
?>
--EXPECT--
int(2)
