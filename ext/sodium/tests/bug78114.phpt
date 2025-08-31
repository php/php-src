--TEST--
Bug #78114 (segfault when calling sodium_* functions from eval)
--EXTENSIONS--
sodium
--FILE--
<?php
try {
    eval('sodium_bin2hex();');
} catch (Throwable $ex) {
    echo $ex->getMessage(), PHP_EOL;
}
?>
--EXPECT--
sodium_bin2hex() expects exactly 1 argument, 0 given
