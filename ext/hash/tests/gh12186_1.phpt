--TEST--
Hash: bug #12186 - segfault in hash_copy() on a finalized context
--FILE--
<?php

$c = hash_init('sha1');
hash_final($c);

try {
    hash_copy($c);
} catch (Throwable $ex) {
    echo $ex->getMessage() . "\n";
}

?>
--EXPECTF--
hash_copy(): Argument #1 ($context) must be a valid, non-finalized HashContext
