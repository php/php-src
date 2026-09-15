--TEST--
GC 048: FE_FREE should mark variable as UNDEF to prevent use-after-free during GC
--FILE--
<?php
// FE_FREE frees the iterator but doesn't set zval to UNDEF
// When GC runs during RETURN, zend_gc_remove_root_tmpvars() may access freed memory

function test_foreach_early_return(string $s): object {
    foreach ((array) $s as $v) {
        $obj = new stdClass;
        // in the early return, the VAR for the cast result is still live
        return $obj; // the return may trigger GC
    }
}

for ($i = 0; $i < 100000; $i++) {
    // create cyclic garbage to fill GC buffer
    $a = new stdClass;
    $b = new stdClass;
    $a->ref = $b;
    $b->ref = $a;

    $result = test_foreach_early_return("x");
}

echo "OK\n";
?>
--EXPECT--
OK
