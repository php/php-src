--TEST--
juliantojd()
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php
echo juliantojd(5, 5, 6000000000) . "\n";
?>
--EXPECT--
622764916319
