--TEST--
Bug #64343 (phar cannot open tars with pax headers)
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip phar extension not loaded"); ?>
--FILE--
<?php

echo "Test\n";

$phar = new PharData(__DIR__."/files/bug64343.tar");

?>
===DONE===
--EXPECT--
Test
===DONE===
