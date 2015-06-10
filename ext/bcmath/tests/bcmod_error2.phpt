--TEST--
bcmod() - mod by 0
--SKIPIF--
<?php if(!extension_loaded("bcmath")) print "skip"; ?>
--INI--
bcmath.scale=0
--FILE--
<?php
try {
    var_dump(bcmod("10", "0"));
} catch (Exception $e) {
    echo "\nException: " . $e->getMessage() . "\n";
}
?>
--EXPECTF--
Exception: Division by zero
