--TEST--
OSS-Fuzz #562721806 (Use-of-uninitialized-value in zim_Time_Duration___unserialize)
--FILE--
<?php
try {
    unserialize('O:13:"Time\Duration"::{}');
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}
?>
--EXPECT--
Exception: Invalid serialization data for Time\Duration object
