--TEST--
Bug #80185 (jdtounix() fails after 2037)
--SKIPIF--
<?php
if (!extension_loaded('calendar')) die('skip ext/calendar required');
if (PHP_INT_SIZE != 4) die("skip for 32bit platforms only");
?>
--FILE--
<?php
try {
    var_dump(jdtounix(2465712));
} catch (ValueError $ex) {
    echo $ex->getMessage(), PHP_EOL;
}
var_dump(jdtounix(PHP_INT_MAX / 86400 + 2440588));
try {
    var_dump(jdtounix(PHP_INT_MAX / 86400 + 2440589));
} catch (ValueError $ex) {
    echo $ex->getMessage(), PHP_EOL;
}
?>
--EXPECT--
jday must be between 2440588 and 2465443
int(2147472000)
jday must be between 2440588 and 2465443
