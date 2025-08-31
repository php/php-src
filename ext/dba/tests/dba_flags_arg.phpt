--TEST--
DBA new flags ValueError test
--EXTENSIONS--
dba
--FILE--
<?php
try {
    dba_open('irrelevant', 'c', 'handler', flags: -1);
} catch (\ValueError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
?>
--EXPECT--
dba_open(): Argument #6 ($flags) must be greater than or equal to 0
