--TEST--
Bug #80185 (jdtounix() fails after 2037)
--EXTENSIONS--
calendar
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip for 64bit platforms only");
?>
--FILE--
<?php
var_dump(jdtounix(2465712));
var_dump(jdtounix((int)(PHP_INT_MAX / 86400 + 2440588)));
try {
    var_dump(jdtounix((int)(PHP_INT_MAX / 86400 + 2440589)));
} catch (ValueError $ex) {
    echo $ex->getMessage(), PHP_EOL;
}
?>
--EXPECT--
int(2170713600)
int(9223372036854720000)
jday must be between 2440588 and 106751993607888
