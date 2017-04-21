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
    var_dump(bcdiv('10.99', '0'));
} catch (Exception $e) {
    echo "\nException: " . $e->getMessage() . "\n";
}
?>
--EXPECTF--
Exception: Division by zero
