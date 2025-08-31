--TEST--
Test jdtounix() function : error conditions
--CREDITS--
edgarsandi - <edgar.r.sandi@gmail.com>
--INI--
date.timezone=UTC
--EXTENSIONS--
calendar
--FILE--
<?php
try {
    jdtounix(2440579);
} catch (ValueError $ex) {
    echo $ex->getMessage(), PHP_EOL;
}
?>
--EXPECTF--
jday must be between 2440588 and %d
