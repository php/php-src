--TEST--
Test the function readline with null parameter (wrong because the function expects a string parameter).
--CREDITS--
Rodrigo Prado de Jesus <royopa [at] gmail [dot] com>
--SKIPIF--
<?php if (!extension_loaded("readline") die("skip"); ?>
--FILE--
<?php
$line = readline(null);
?>
--EXPECTF--
