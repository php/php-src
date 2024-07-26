--TEST--
Bug #78694 (Appending to a variant array causes segfault)
--EXTENSIONS--
com_dotnet
--FILE--
<?php
foreach ([new com('WScript.Shell'), new variant([])] as $var) {
    try {
        $var[] = 42;
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), PHP_EOL;
    }
}
?>
--EXPECT--
Error: Cannot append to object of type com
Error: Cannot append to object of type variant
