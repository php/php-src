--TEST--
bcdiv — Divide two arbitrary precision numbers
--CREDITS--
TestFest2009
Antoni Torrents
antoni@solucionsinternet.com
--SKIPIF--
<?php if(!extension_loaded("bcmath")) print "skip"; ?>
--FILE--
<?php
try {
    bcdiv('10.99', '0');
} catch (DivisionByZeroError $ex) {
    echo $ex->getMessage(), PHP_EOL;
}
?>
--EXPECT--
Division by zero
