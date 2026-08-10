--TEST--
Bug #81714 (segfault when serializing finalized HashContext)
--FILE--
<?php
$h = hash_init('md5');
hash_final($h);
try {
    serialize($h);
} catch (Exception $ex) {
    echo $ex::class, ': ', $ex->getMessage(), "\n";
}
?>
--EXPECTF--
Exception: HashContext for algorithm "md5" cannot be serialized
