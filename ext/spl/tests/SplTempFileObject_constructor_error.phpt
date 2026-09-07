--TEST--
SPL SplTempFileObject constructor sets correct defaults when pass 0 arguments
--FILE--
<?php
try {
    new SplTempFileObject('invalid');
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
TypeError: SplTempFileObject::__construct(): Argument #1 ($maxMemory) must be of type int, string given
