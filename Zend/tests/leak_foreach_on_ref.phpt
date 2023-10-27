--TEST--
Fix memory leak if a circular reference is created inside a by-ref foreach on the array variable.
--FILE--
<?php

$a = [0];

foreach ($a as &$v) {
    $a[0] = &$a;
    unset($a);
    gc_collect_cycles();
}

?>
==DONE==
--EXPECT--
==DONE==
