--TEST--
Bug #81714 (segfault when serializing finalized HashContext)
--FILE--
<?php
$h = hash_init('md5');
hash_final($h);
try {
    serialize($h);
} catch (Exception $ex) {
    var_dump($ex->getMessage());
}
?>
--EXPECTF--
string(52) "HashContext for algorithm "md5" cannot be serialized"
