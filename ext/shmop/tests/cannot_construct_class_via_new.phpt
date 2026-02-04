--TEST--
Shmop class must not be instantiatable
--EXTENSIONS--
shmop
--FILE--
<?php
try {
    var_dump(new Shmop());
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
Error: Cannot directly construct Shmop, use shmop_open() instead
