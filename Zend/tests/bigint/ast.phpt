--TEST--
Bigint AST representation in assert()
--SKIPIF--
<?php if (!extension_loaded('gmp')) { die('skip requires gmp'); } ?>
--FILE--
<?php
// The result is a GMP arbitrary precision number object, not an int
try {
    assert(is_int(1+0n+01_23n));
} catch (AssertionError $e) {
    echo "Caught " . $e->getMessage() . "\n";
}

?>
--EXPECTF--
Caught assert(is_int(1 + 0n + 01_23n))
