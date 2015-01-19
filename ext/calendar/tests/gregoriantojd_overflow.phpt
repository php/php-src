--TEST--
gregoriantojd()
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php
echo gregoriantojd(5, 5, 6000000) . "\n";
?>
--EXPECT--
2193176185
